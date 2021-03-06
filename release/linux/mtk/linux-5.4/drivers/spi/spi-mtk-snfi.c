// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for MediaTek SPI Nand interface
 *
 * Copyright (C) 2018 MediaTek Inc.
 * Authors:	Xiangsheng Hou	<xiangsheng.hou@mediatek.com>
 *
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/iopoll.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/mtk_ecc.h>
#include <linux/mtd/spinand.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi-mem.h>

/* NAND controller register definition */
/* NFI control */
#define NFI_CNFG		0x00
#define		CNFG_DMA		BIT(0)
#define		CNFG_READ_EN		BIT(1)
#define		CNFG_DMA_BURST_EN	BIT(2)
#define		CNFG_BYTE_RW		BIT(6)
#define		CNFG_HW_ECC_EN		BIT(8)
#define		CNFG_AUTO_FMT_EN	BIT(9)
#define		CNFG_OP_PROGRAM		(3UL << 12)
#define		CNFG_OP_CUST		(6UL << 12)
#define NFI_PAGEFMT		0x04
#define		PAGEFMT_512		0
#define		PAGEFMT_2K		1
#define		PAGEFMT_4K		2
#define		PAGEFMT_FDM_SHIFT	8
#define		PAGEFMT_FDM_ECC_SHIFT	12
#define NFI_CON			0x08
#define		CON_FIFO_FLUSH		BIT(0)
#define		CON_NFI_RST		BIT(1)
#define		CON_BRD			BIT(8)
#define		CON_BWR			BIT(9)
#define		CON_SEC_SHIFT		12
#define NFI_INTR_EN		0x10
#define		INTR_AHB_DONE_EN	BIT(6)
#define NFI_INTR_STA		0x14
#define NFI_CMD			0x20
#define NFI_STA			0x60
#define		STA_EMP_PAGE		BIT(12)
#define		NAND_FSM_MASK		(0x1f << 24)
#define		NFI_FSM_MASK		(0xf << 16)
#define NFI_ADDRCNTR		0x70
#define		CNTR_MASK		GENMASK(16, 12)
#define		ADDRCNTR_SEC_SHIFT	12
#define		ADDRCNTR_SEC(val) \
		(((val) & CNTR_MASK) >> ADDRCNTR_SEC_SHIFT)
#define NFI_STRADDR		0x80
#define NFI_BYTELEN		0x84
#define NFI_CSEL		0x90
#define NFI_FDML(x)		(0xa0 + (x) * sizeof(u32) * 2)
#define NFI_FDMM(x)		(0xa4 + (x) * sizeof(u32) * 2)
#define NFI_MASTER_STA		0x224
#define		MASTER_STA_MASK		0x0fff
/* NFI_SPI control */
#define SNFI_MAC_OUTL		0x504
#define SNFI_MAC_INL		0x508
#define SNFI_RD_CTL2		0x510
#define		RD_CMD_MASK		0x00ff
#define		RD_DUMMY_SHIFT		8
#define SNFI_RD_CTL3		0x514
#define		RD_ADDR_MASK		0xffff
#define SNFI_MISC_CTL		0x538
#define		RD_MODE_X2		BIT(16)
#define		RD_MODE_X4		(2UL << 16)
#define		RD_QDUAL_IO		(4UL << 16)
#define		RD_MODE_MASK		(7UL << 16)
#define		RD_CUSTOM_EN		BIT(6)
#define		WR_CUSTOM_EN		BIT(7)
#define		WR_X4_EN		BIT(20)
#define		SW_RST			BIT(28)
#define SNFI_MISC_CTL2		0x53c
#define		WR_LEN_SHIFT		16
#define SNFI_PG_CTL1		0x524
#define		WR_LOAD_CMD_SHIFT	8
#define SNFI_PG_CTL2		0x528
#define		WR_LOAD_ADDR_MASK	0xffff
#define SNFI_MAC_CTL		0x500
#define		MAC_WIP			BIT(0)
#define		MAC_WIP_READY		BIT(1)
#define		MAC_TRIG		BIT(2)
#define		MAC_EN			BIT(3)
#define		MAC_SIO_SEL		BIT(4)
#define SNFI_STA_CTL1		0x550
#define		SPI_STATE_IDLE		0xf
#define SNFI_CNFG		0x55c
#define		SNFI_MODE_EN		BIT(0)
#define SNFI_GPRAM_DATA		0x800
#define		SNFI_GPRAM_MAX_LEN	16

/* Dummy command trigger NFI to spi mode */
#define NAND_CMD_DUMMYREAD	0x00
#define NAND_CMD_DUMMYPROG	0x80

#define MTK_TIMEOUT		500000
#define MTK_RESET_TIMEOUT	1000000
#define MTK_SNFC_MIN_SPARE	16
#define KB(x)			((x) * 1024UL)

/*
 * supported spare size of each IP.
 * order should be the same with the spare size bitfiled defination of
 * register NFI_PAGEFMT.
 */
static const u8 spare_size_mt7622[] = {
	16, 26, 27, 28
};

struct mtk_snfi_caps {
	const u8 *spare_size;
	u8 num_spare_size;
	u32 nand_sec_size;
	u8 nand_fdm_size;
	u8 nand_fdm_ecc_size;
	u8 ecc_parity_bits;
	u8 pageformat_spare_shift;
	u8 bad_mark_swap;
};

struct mtk_snfi_bad_mark_ctl {
	void (*bm_swap)(struct spi_mem *mem, u8 *buf, int raw);
	u32 sec;
	u32 pos;
};

struct mtk_snfi_nand_chip {
	struct mtk_snfi_bad_mark_ctl bad_mark;
	u32 spare_per_sector;
};

struct mtk_snfi_clk {
	struct clk *nfi_clk;
	struct clk *spi_clk;
};

struct mtk_snfi {
	const struct mtk_snfi_caps *caps;
	struct mtk_snfi_nand_chip snfi_nand;
	struct mtk_snfi_clk clk;
	struct mtk_ecc_config ecc_cfg;
	struct mtk_ecc *ecc;
	struct completion done;
	struct device *dev;

	void __iomem *regs;

	u8 *buffer;
};

static inline u8 *oob_ptr(struct spi_mem *mem, int i)
{
	struct spinand_device *spinand = spi_mem_get_drvdata(mem);
	struct mtk_snfi *snfi = spi_controller_get_devdata(mem->spi->master);
	struct mtk_snfi_nand_chip *snfi_nand = &snfi->snfi_nand;
	u8 *poi;

	/* map the sector's FDM data to free oob:
	 * the beginning of the oob area stores the FDM data of bad mark
	 */

	if (i < snfi_nand->bad_mark.sec)
		poi = spinand->oobbuf + (i + 1) * snfi->caps->nand_fdm_size;
	else if (i == snfi_nand->bad_mark.sec)
		poi = spinand->oobbuf;
	else
		poi = spinand->oobbuf + i * snfi->caps->nand_fdm_size;

	return poi;
}

static inline int mtk_data_len(struct spi_mem *mem)
{
	struct mtk_snfi *snfi = spi_controller_get_devdata(mem->spi->master);
	struct mtk_snfi_nand_chip *snfi_nand = &snfi->snfi_nand;

	return snfi->caps->nand_sec_size + snfi_nand->spare_per_sector;
}

static inline u8 *mtk_oob_ptr(struct spi_mem *mem,
			      const u8 *p, int i)
{
	struct mtk_snfi *snfi = spi_controller_get_devdata(mem->spi->master);

	return (u8 *)p + i * mtk_data_len(mem) + snfi->caps->nand_sec_size;
}

static void mtk_snfi_bad_mark_swap(struct spi_mem *mem,
				   u8 *buf, int raw)
{
	struct spinand_device *spinand = spi_mem_get_drvdata(mem);
	struct mtk_snfi *snfi = spi_controller_get_devdata(mem->spi->master);
	struct mtk_snfi_nand_chip *snfi_nand = &snfi->snfi_nand;
	u32 bad_pos = snfi_nand->bad_mark.pos;

	if (raw)
		bad_pos += snfi_nand->bad_mark.sec * mtk_data_len(mem);
	else
		bad_pos += snfi_nand->bad_mark.sec * snfi->caps->nand_sec_size;

	swap(spinand->oobbuf[0], buf[bad_pos]);
}

static void mtk_snfi_set_bad_mark_ctl(struct mtk_snfi_bad_mark_ctl *bm_ctl,
				      struct spi_mem *mem)
{
	struct spinand_device *spinand = spi_mem_get_drvdata(mem);
	struct mtd_info *mtd = spinand_to_mtd(spinand);

	bm_ctl->bm_swap = mtk_snfi_bad_mark_swap;
	bm_ctl->sec = mtd->writesize / mtk_data_len(mem);
	bm_ctl->pos = mtd->writesize % mtk_data_len(mem);
}

static void mtk_snfi_mac_enable(struct mtk_snfi *snfi)
{
	u32 mac;

	mac = readl(snfi->regs + SNFI_MAC_CTL);
	mac &= ~MAC_SIO_SEL;
	mac |= MAC_EN;

	writel(mac, snfi->regs + SNFI_MAC_CTL);
}

static int mtk_snfi_mac_trigger(struct mtk_snfi *snfi)
{
	u32 mac, reg;
	int ret = 0;

	mac = readl(snfi->regs + SNFI_MAC_CTL);
	mac |= MAC_TRIG;
	writel(mac, snfi->regs + SNFI_MAC_CTL);

	ret = readl_poll_timeout_atomic(snfi->regs + SNFI_MAC_CTL, reg,
					reg & MAC_WIP_READY, 10,
					MTK_TIMEOUT);
	if (ret < 0) {
		dev_err(snfi->dev, "polling wip ready for read timeout\n");
		return -EIO;
	}

	ret = readl_poll_timeout_atomic(snfi->regs + SNFI_MAC_CTL, reg,
					!(reg & MAC_WIP), 10,
					MTK_TIMEOUT);
	if (ret < 0) {
		dev_err(snfi->dev, "polling flash update timeout\n");
		return -EIO;
	}

	return ret;
}

static void mtk_snfi_mac_leave(struct mtk_snfi *snfi)
{
	u32 mac;

	mac = readl(snfi->regs + SNFI_MAC_CTL);
	mac &= ~(MAC_TRIG | MAC_EN | MAC_SIO_SEL);
	writel(mac, snfi->regs + SNFI_MAC_CTL);
}

static int mtk_snfi_mac_op(struct mtk_snfi *snfi)
{
	int ret = 0;

	mtk_snfi_mac_enable(snfi);

	ret = mtk_snfi_mac_trigger(snfi);
	if (ret)
		return ret;

	mtk_snfi_mac_leave(snfi);

	return ret;
}

static irqreturn_t mtk_snfi_irq(int irq, void *id)
{
	struct mtk_snfi *snfi = id;
	u16 sta, ien;

	sta = readw(snfi->regs + NFI_INTR_STA);
	ien = readw(snfi->regs + NFI_INTR_EN);

	if (!(sta & ien))
		return IRQ_NONE;

	writew(~sta & ien, snfi->regs + NFI_INTR_EN);
	complete(&snfi->done);

	return IRQ_HANDLED;
}

static int mtk_snfi_enable_clk(struct device *dev, struct mtk_snfi_clk *clk)
{
	int ret;

	ret = clk_prepare_enable(clk->nfi_clk);
	if (ret) {
		dev_err(dev, "failed to enable nfi clk\n");
		return ret;
	}

	ret = clk_prepare_enable(clk->spi_clk);
	if (ret) {
		dev_err(dev, "failed to enable spi clk\n");
		clk_disable_unprepare(clk->nfi_clk);
		return ret;
	}

	return 0;
}

static void mtk_snfi_disable_clk(struct mtk_snfi_clk *clk)
{
	clk_disable_unprepare(clk->nfi_clk);
	clk_disable_unprepare(clk->spi_clk);
}

static int mtk_snfi_reset(struct mtk_snfi *snfi)
{
	u32 val;
	int ret;

	/* SW reset controller */
	val = readl(snfi->regs + SNFI_MISC_CTL) | SW_RST;
	writel(val, snfi->regs + SNFI_MISC_CTL);

	ret = readw_poll_timeout(snfi->regs + SNFI_STA_CTL1, val,
				 !(val & SPI_STATE_IDLE), 50,
				 MTK_RESET_TIMEOUT);
	if (ret) {
		dev_warn(snfi->dev, "spi state active in reset [0x%x] = 0x%x\n",
			 SNFI_STA_CTL1, val);
		return ret;
	}

	val = readl(snfi->regs + SNFI_MISC_CTL);
	val &= ~SW_RST;
	writel(val, snfi->regs + SNFI_MISC_CTL);

	/* reset all registers and force the NFI master to terminate */
	writew(CON_FIFO_FLUSH | CON_NFI_RST, snfi->regs + NFI_CON);
	ret = readw_poll_timeout(snfi->regs + NFI_STA, val,
				 !(val & (NFI_FSM_MASK | NAND_FSM_MASK)), 50,
				 MTK_RESET_TIMEOUT);
	if (ret) {
		dev_warn(snfi->dev, "nfi active in reset [0x%x] = 0x%x\n",
			 NFI_STA, val);
		return ret;
	}

	return 0;
}

static int mtk_snfi_set_spare_per_sector(struct spinand_device *spinand,
					 const struct mtk_snfi_caps *caps,
					 u32 *sps)
{
	struct mtd_info *mtd = spinand_to_mtd(spinand);
	const u8 *spare = caps->spare_size;
	u32 sectors, i, closest_spare = 0;

	sectors = mtd->writesize / caps->nand_sec_size;
	*sps = mtd->oobsize / sectors;

	if (*sps < MTK_SNFC_MIN_SPARE)
		return -EINVAL;

	for (i = 0; i < caps->num_spare_size; i++) {
		if (*sps >= spare[i] && spare[i] >= spare[closest_spare]) {
			closest_spare = i;
			if (*sps == spare[i])
				break;
		}
	}

	*sps = spare[closest_spare];

	return 0;
}

static void mtk_snfi_read_fdm_data(struct spi_mem *mem,
				   u32 sectors)
{
	struct mtk_snfi *snfi = spi_controller_get_devdata(mem->spi->master);
	const struct mtk_snfi_caps *caps = snfi->caps;
	u32 vall, valm;
	int i, j;
	u8 *oobptr;

	for (i = 0; i < sectors; i++) {
		oobptr = oob_ptr(mem, i);
		vall = readl(snfi->regs + NFI_FDML(i));
		valm = readl(snfi->regs + NFI_FDMM(i));

		for (j = 0; j < caps->nand_fdm_size; j++)
			oobptr[j] = (j >= 4 ? valm : vall) >> ((j % 4) * 8);
	}
}

static void mtk_snfi_write_fdm_data(struct spi_mem *mem,
				    u32 sectors)
{
	struct mtk_snfi *snfi = spi_controller_get_devdata(mem->spi->master);
	const struct mtk_snfi_caps *caps = snfi->caps;
	u32 vall, valm;
	int i, j;
	u8 *oobptr;

	for (i = 0; i < sectors; i++) {
		oobptr = oob_ptr(mem, i);
		vall = 0;
		valm = 0;
		for (j = 0; j < 8; j++) {
			if (j < 4)
				vall |= (j < caps->nand_fdm_size ? oobptr[j] :
					 0xff) << (j * 8);
			else
				valm |= (j < caps->nand_fdm_size ? oobptr[j] :
					 0xff) << ((j - 4) * 8);
		}
		writel(vall, snfi->regs + NFI_FDML(i));
		writel(valm, snfi->regs + NFI_FDMM(i));
	}
}

static int mtk_snfi_update_ecc_stats(struct spi_mem *mem,
				     u8 *buf, u32 sectors)
{
	struct spinand_device *spinand = spi_mem_get_drvdata(mem);
	struct mtd_info *mtd = spinand_to_mtd(spinand);
	struct mtk_snfi *snfi = spi_controller_get_devdata(mem->spi->master);
	struct mtk_ecc_stats stats;
	int rc, i;

	rc = readl(snfi->regs + NFI_STA) & STA_EMP_PAGE;
	if (rc) {
		memset(buf, 0xff, sectors * snfi->caps->nand_sec_size);
		for (i = 0; i < sectors; i++)
			memset(spinand->oobbuf, 0xff,
			       snfi->caps->nand_fdm_size);
		return 0;
	}

	mtk_ecc_get_stats(snfi->ecc, &stats, sectors);
	mtd->ecc_stats.corrected += stats.corrected;
	mtd->ecc_stats.failed += stats.failed;

	return 0;
}

static int mtk_snfi_hw_runtime_config(struct spi_mem *mem)
{
	struct spinand_device *spinand = spi_mem_get_drvdata(mem);
	struct mtd_info *mtd = spinand_to_mtd(spinand);
	struct nand_device *nand = mtd_to_nanddev(mtd);
	struct mtk_snfi *snfi = spi_controller_get_devdata(mem->spi->master);
	const struct mtk_snfi_caps *caps = snfi->caps;
	struct mtk_snfi_nand_chip *snfi_nand = &snfi->snfi_nand;
	u32 fmt, spare, i = 0;
	int ret;

	ret = mtk_snfi_set_spare_per_sector(spinand, caps, &spare);
	if (ret)
		return ret;

	/* calculate usable oob bytes for ecc parity data */
	snfi_nand->spare_per_sector = spare;
	spare -= caps->nand_fdm_size;

	nand->memorg.oobsize = snfi_nand->spare_per_sector
		* (mtd->writesize / caps->nand_sec_size);
	mtd->oobsize = nanddev_per_page_oobsize(nand);

	snfi->ecc_cfg.strength = (spare << 3) / caps->ecc_parity_bits;
	mtk_ecc_adjust_strength(snfi->ecc, &snfi->ecc_cfg.strength);

	switch (mtd->writesize) {
	case 512:
		fmt = PAGEFMT_512;
		break;
	case KB(2):
		fmt = PAGEFMT_2K;
		break;
	case KB(4):
		fmt = PAGEFMT_4K;
		break;
	default:
		dev_err(snfi->dev, "invalid page len: %d\n", mtd->writesize);
		return -EINVAL;
	}

	/* Setup PageFormat */
	while (caps->spare_size[i] != snfi_nand->spare_per_sector) {
		i++;
		if (i == (caps->num_spare_size - 1)) {
			dev_err(snfi->dev, "invalid spare size %d\n",
				snfi_nand->spare_per_sector);
			return -EINVAL;
		}
	}

	fmt |= i << caps->pageformat_spare_shift;
	fmt |= caps->nand_fdm_size << PAGEFMT_FDM_SHIFT;
	fmt |= caps->nand_fdm_ecc_size << PAGEFMT_FDM_ECC_SHIFT;
	writel(fmt, snfi->regs + NFI_PAGEFMT);

	snfi->ecc_cfg.len = caps->nand_sec_size + caps->nand_fdm_ecc_size;

	mtk_snfi_set_bad_mark_ctl(&snfi_nand->bad_mark, mem);

	return 0;
}

static int mtk_snfi_read_from_cache(struct spi_mem *mem,
				    const struct spi_mem_op *op, int oob_on)
{
	struct mtk_snfi *snfi = spi_controller_get_devdata(mem->spi->master);
	struct spinand_device *spinand = spi_mem_get_drvdata(mem);
	struct mtd_info *mtd = spinand_to_mtd(spinand);
	u32 sectors = mtd->writesize / snfi->caps->nand_sec_size;
	struct mtk_snfi_nand_chip *snfi_nand = &snfi->snfi_nand;
	u32 reg, len, col_addr = 0;
	int dummy_cycle, ret;
	dma_addr_t dma_addr;

	len = sectors * (snfi->caps->nand_sec_size
	      + snfi_nand->spare_per_sector);

	dma_addr = dma_map_single(snfi->dev, snfi->buffer,
				  len, DMA_FROM_DEVICE);
	ret = dma_mapping_error(snfi->dev, dma_addr);
	if (ret) {
		dev_err(snfi->dev, "dma mapping error\n");
		return -EINVAL;
	}

	/* set Read cache command and dummy cycle */
	dummy_cycle = (op->dummy.nbytes << 3) >> (ffs(op->dummy.buswidth) - 1);
	reg = ((op->cmd.opcode & RD_CMD_MASK) |
	       (dummy_cycle << RD_DUMMY_SHIFT));
	writel(reg, snfi->regs + SNFI_RD_CTL2);

	writel((col_addr & RD_ADDR_MASK), snfi->regs + SNFI_RD_CTL3);

	reg = readl(snfi->regs + SNFI_MISC_CTL);
	reg |= RD_CUSTOM_EN;
	reg &= ~(RD_MODE_MASK | WR_X4_EN);

	/* set data and addr buswidth */
	if (op->data.buswidth == 4)
		reg |= RD_MODE_X4;
	else if (op->data.buswidth == 2)
		reg |= RD_MODE_X2;

	if (op->addr.buswidth == 4 || op->addr.buswidth == 2)
		reg |= RD_QDUAL_IO;
	writel(reg, snfi->regs + SNFI_MISC_CTL);

	writel(len, snfi->regs + SNFI_MISC_CTL2);
	writew(sectors << CON_SEC_SHIFT, snfi->regs + NFI_CON);
	reg = readw(snfi->regs + NFI_CNFG);
	reg |= CNFG_READ_EN | CNFG_DMA_BURST_EN | CNFG_DMA | CNFG_OP_CUST;

	if (!oob_on) {
		reg |= CNFG_AUTO_FMT_EN | CNFG_HW_ECC_EN;
		writew(reg, snfi->regs + NFI_CNFG);

		snfi->ecc_cfg.mode = ECC_NFI_MODE;
		snfi->ecc_cfg.sectors = sectors;
		snfi->ecc_cfg.op = ECC_DECODE;
		ret = mtk_ecc_enable(snfi->ecc, &snfi->ecc_cfg);
		if (ret) {
			dev_err(snfi->dev, "ecc enable failed\n");
			/* clear NFI_CNFG */
			reg &= ~(CNFG_READ_EN | CNFG_DMA_BURST_EN | CNFG_DMA |
				CNFG_AUTO_FMT_EN | CNFG_HW_ECC_EN);
			writew(reg, snfi->regs + NFI_CNFG);
			goto out;
		}
	} else {
		writew(reg, snfi->regs + NFI_CNFG);
	}

	writel(lower_32_bits(dma_addr), snfi->regs + NFI_STRADDR);
	readw(snfi->regs + NFI_INTR_STA);
	writew(INTR_AHB_DONE_EN, snfi->regs + NFI_INTR_EN);

	init_completion(&snfi->done);

	/* set dummy command to trigger NFI enter SPI mode */
	writew(NAND_CMD_DUMMYREAD, snfi->regs + NFI_CMD);
	reg = readl(snfi->regs + NFI_CON) | CON_BRD;
	writew(reg, snfi->regs + NFI_CON);

	ret = wait_for_completion_timeout(&snfi->done, msecs_to_jiffies(500));
	if (!ret) {
		dev_err(snfi->dev, "read ahb done timeout\n");
		writew(0, snfi->regs + NFI_INTR_EN);
		ret = -ETIMEDOUT;
		goto out;
	}

	ret = readl_poll_timeout_atomic(snfi->regs + NFI_BYTELEN, reg,
					ADDRCNTR_SEC(reg) >= sectors, 10,
					MTK_TIMEOUT);
	if (ret < 0) {
		dev_err(snfi->dev, "polling read byte len timeout\n");
		ret = -EIO;
	} else {
		if (!oob_on) {
			ret = mtk_ecc_wait_done(snfi->ecc, ECC_DECODE);
			if (ret) {
				dev_warn(snfi->dev, "wait ecc done timeout\n");
			} else {
				mtk_snfi_update_ecc_stats(mem, snfi->buffer,
							  sectors);
				mtk_snfi_read_fdm_data(mem, sectors);
			}
		}
	}

	if (oob_on)
		goto out;

	mtk_ecc_disable(snfi->ecc);
out:
	dma_unmap_single(snfi->dev, dma_addr, len, DMA_FROM_DEVICE);
	writel(0, snfi->regs + NFI_CON);
	writel(0, snfi->regs + NFI_CNFG);
	reg = readl(snfi->regs + SNFI_MISC_CTL);
	reg &= ~RD_CUSTOM_EN;
	writel(reg, snfi->regs + SNFI_MISC_CTL);

	return ret;
}

static int mtk_snfi_write_to_cache(struct spi_mem *mem,
				   const struct spi_mem_op *op,
				   int oob_on)
{
	struct mtk_snfi *snfi = spi_controller_get_devdata(mem->spi->master);
	struct spinand_device *spinand = spi_mem_get_drvdata(mem);
	struct mtd_info *mtd = spinand_to_mtd(spinand);
	u32 sectors = mtd->writesize / snfi->caps->nand_sec_size;
	struct mtk_snfi_nand_chip *snfi_nand = &snfi->snfi_nand;
	u32 reg, len, col_addr = 0;
	dma_addr_t dma_addr;
	int ret;

	len = sectors * (snfi->caps->nand_sec_size
	      + snfi_nand->spare_per_sector);

	dma_addr = dma_map_single(snfi->dev, snfi->buffer, len,
				  DMA_TO_DEVICE);
	ret = dma_mapping_error(snfi->dev, dma_addr);
	if (ret) {
		dev_err(snfi->dev, "dma mapping error\n");
		return -EINVAL;
	}

	/* set program load cmd and address */
	reg = (op->cmd.opcode << WR_LOAD_CMD_SHIFT);
	writel(reg, snfi->regs + SNFI_PG_CTL1);
	writel(col_addr & WR_LOAD_ADDR_MASK, snfi->regs + SNFI_PG_CTL2);

	reg = readl(snfi->regs + SNFI_MISC_CTL);
	reg |= WR_CUSTOM_EN;
	reg &= ~(RD_MODE_MASK | WR_X4_EN);

	if (op->data.buswidth == 4)
		reg |= WR_X4_EN;
	writel(reg, snfi->regs + SNFI_MISC_CTL);

	writel(len << WR_LEN_SHIFT, snfi->regs + SNFI_MISC_CTL2);
	writew(sectors << CON_SEC_SHIFT, snfi->regs + NFI_CON);

	reg = readw(snfi->regs + NFI_CNFG);
	reg &= ~(CNFG_READ_EN | CNFG_BYTE_RW);
	reg |= CNFG_DMA | CNFG_DMA_BURST_EN | CNFG_OP_PROGRAM;

	if (!oob_on) {
		reg |= CNFG_AUTO_FMT_EN | CNFG_HW_ECC_EN;
		writew(reg, snfi->regs + NFI_CNFG);

		snfi->ecc_cfg.mode = ECC_NFI_MODE;
		snfi->ecc_cfg.op = ECC_ENCODE;
		ret = mtk_ecc_enable(snfi->ecc, &snfi->ecc_cfg);
		if (ret) {
			dev_err(snfi->dev, "ecc enable failed\n");
			/* clear NFI_CNFG */
			reg &= ~(CNFG_DMA_BURST_EN | CNFG_DMA |
					CNFG_AUTO_FMT_EN | CNFG_HW_ECC_EN);
			writew(reg, snfi->regs + NFI_CNFG);
			dma_unmap_single(snfi->dev, dma_addr, len,
					 DMA_FROM_DEVICE);
			goto out;
		}
		/* write OOB into the FDM registers (OOB area in MTK NAND) */
		mtk_snfi_write_fdm_data(mem, sectors);
	} else {
		writew(reg, snfi->regs + NFI_CNFG);
	}
	writel(lower_32_bits(dma_addr), snfi->regs + NFI_STRADDR);
	readw(snfi->regs + NFI_INTR_STA);
	writew(INTR_AHB_DONE_EN, snfi->regs + NFI_INTR_EN);

	init_completion(&snfi->done);

	/* set dummy command to trigger NFI enter SPI mode */
	writew(NAND_CMD_DUMMYPROG, snfi->regs + NFI_CMD);
	reg = readl(snfi->regs + NFI_CON) | CON_BWR;
	writew(reg, snfi->regs + NFI_CON);

	ret = wait_for_completion_timeout(&snfi->done, msecs_to_jiffies(500));
	if (!ret) {
		dev_err(snfi->dev, "custom program done timeout\n");
		writew(0, snfi->regs + NFI_INTR_EN);
		ret = -ETIMEDOUT;
		goto ecc_disable;
	}

	ret = readl_poll_timeout_atomic(snfi->regs + NFI_ADDRCNTR, reg,
					ADDRCNTR_SEC(reg) >= sectors,
					10, MTK_TIMEOUT);
	if (ret)
		dev_err(snfi->dev, "hwecc write timeout\n");

ecc_disable:
	mtk_ecc_disable(snfi->ecc);

out:
	dma_unmap_single(snfi->dev, dma_addr, len, DMA_TO_DEVICE);
	writel(0, snfi->regs + NFI_CON);
	writel(0, snfi->regs + NFI_CNFG);
	reg = readl(snfi->regs + SNFI_MISC_CTL);
	reg &= ~WR_CUSTOM_EN;
	writel(reg, snfi->regs + SNFI_MISC_CTL);

	return ret;
}

static int mtk_snfi_read(struct spi_mem *mem,
			 const struct spi_mem_op *op)
{
	struct spinand_device *spinand = spi_mem_get_drvdata(mem);
	struct mtk_snfi *snfi = spi_controller_get_devdata(mem->spi->master);
	struct mtd_info *mtd = spinand_to_mtd(spinand);
	struct mtk_snfi_nand_chip *snfi_nand = &snfi->snfi_nand;
	u32 col_addr = op->addr.val;
	int i, ret, sectors, oob_on = false;

	if (col_addr == mtd->writesize)
		oob_on = true;

	ret = mtk_snfi_read_from_cache(mem, op, oob_on);
	if (ret) {
		dev_warn(snfi->dev, "read from cache fail\n");
		return ret;
	}

	sectors = mtd->writesize / snfi->caps->nand_sec_size;
	for (i = 0; i < sectors; i++) {
		if (oob_on)
			memcpy(oob_ptr(mem, i),
			       mtk_oob_ptr(mem, snfi->buffer, i),
			       snfi->caps->nand_fdm_size);

		if (i == snfi_nand->bad_mark.sec && snfi->caps->bad_mark_swap)
			snfi_nand->bad_mark.bm_swap(mem, snfi->buffer,
							oob_on);
	}

	if (!oob_on)
		memcpy(spinand->databuf, snfi->buffer, mtd->writesize);

	return ret;
}

static int mtk_snfi_write(struct spi_mem *mem,
			  const struct spi_mem_op *op)
{
	struct spinand_device *spinand = spi_mem_get_drvdata(mem);
	struct mtk_snfi *snfi = spi_controller_get_devdata(mem->spi->master);
	struct mtd_info *mtd = spinand_to_mtd(spinand);
	struct mtk_snfi_nand_chip *snfi_nand = &snfi->snfi_nand;
	u32 ret, i, sectors, col_addr = op->addr.val;
	int oob_on = false;

	if (col_addr == mtd->writesize)
		oob_on = true;

	sectors = mtd->writesize / snfi->caps->nand_sec_size;
	memset(snfi->buffer, 0xff, mtd->writesize + mtd->oobsize);

	if (!oob_on)
		memcpy(snfi->buffer, spinand->databuf, mtd->writesize);

	for (i = 0; i < sectors; i++) {
		if (i == snfi_nand->bad_mark.sec && snfi->caps->bad_mark_swap)
			snfi_nand->bad_mark.bm_swap(mem, snfi->buffer, oob_on);

		if (oob_on)
			memcpy(mtk_oob_ptr(mem, snfi->buffer, i),
			       oob_ptr(mem, i),
			       snfi->caps->nand_fdm_size);
	}

	ret = mtk_snfi_write_to_cache(mem, op, oob_on);
	if (ret)
		dev_warn(snfi->dev, "write to cache fail\n");

	return ret;
}

static int mtk_snfi_command_exec(struct mtk_snfi *snfi,
				 const u8 *txbuf, u8 *rxbuf,
				 const u32 txlen, const u32 rxlen)
{
	u32 tmp, i, j, reg, m;
	u8 *p_tmp = (u8 *)(&tmp);
	int ret = 0;

	/* Moving tx data to NFI_SPI GPRAM */
	for (i = 0, m = 0; i < txlen; ) {
		for (j = 0, tmp = 0; i < txlen && j < 4; i++, j++)
			p_tmp[j] = txbuf[i];

		writel(tmp, snfi->regs + SNFI_GPRAM_DATA + m);
		m += 4;
	}

	writel(txlen, snfi->regs + SNFI_MAC_OUTL);
	writel(rxlen, snfi->regs + SNFI_MAC_INL);
	ret = mtk_snfi_mac_op(snfi);
	if (ret)
		return ret;

	/* For NULL input data, this loop will be skipped */
	if (rxlen)
		for (i = 0, m = 0; i < rxlen; ) {
			reg = readl(snfi->regs +
					    SNFI_GPRAM_DATA + m);
			for (j = 0; i < rxlen && j < 4; i++, j++, rxbuf++) {
				if (m == 0 && i == 0)
					j = i + txlen;
				*rxbuf = (reg >> (j * 8)) & 0xFF;
			}
			m += 4;
		}

	return ret;
}

/*
 * mtk_snfi_exec_op - to process command/data to send to the
 * SPI NAND by mtk controller
 */
static int mtk_snfi_exec_op(struct spi_mem *mem,
			    const struct spi_mem_op *op)

{
	struct mtk_snfi *snfi = spi_controller_get_devdata(mem->spi->master);
	struct spinand_device *spinand = spi_mem_get_drvdata(mem);
	struct mtd_info *mtd = spinand_to_mtd(spinand);
	struct nand_device *nand = mtd_to_nanddev(mtd);
	const struct spi_mem_op *read_cache;
	const struct spi_mem_op *write_cache;
	const struct spi_mem_op *update_cache;
	u32 tmpbufsize, txlen = 0, rxlen = 0;
	u8 *txbuf, *rxbuf = NULL, *buf;
	int i, ret = 0;
	
	ret = mtk_snfi_reset(snfi);
	if (ret) {
		dev_warn(snfi->dev, "reset spi memory controller fail\n");
		return ret;
	}

	/*if bbt initial, framework have detect nand information */
	if (nand->bbt.cache) {
		read_cache = spinand->op_templates.read_cache;
		write_cache = spinand->op_templates.write_cache;
		update_cache = spinand->op_templates.update_cache;

		ret = mtk_snfi_hw_runtime_config(mem);
		if (ret)
			return ret;

		/* For Read/Write with cache, Erase use framework flow */
		if (op->cmd.opcode == read_cache->cmd.opcode) {
			ret = mtk_snfi_read(mem, op);
			if (ret)
				dev_warn(snfi->dev, "snfi read fail\n");
			
			return ret;
		} else if ((op->cmd.opcode == write_cache->cmd.opcode)
			       	|| (op->cmd.opcode == update_cache->cmd.opcode)) {
			ret = mtk_snfi_write(mem, op);
			if (ret)
				dev_warn(snfi->dev, "snfi write fail\n");
			
			return ret;
		}
	}

	tmpbufsize = sizeof(op->cmd.opcode) + op->addr.nbytes +
		     op->dummy.nbytes + op->data.nbytes;

	txbuf = kzalloc(tmpbufsize, GFP_KERNEL);
	if (!txbuf)
		return -ENOMEM;

	txbuf[txlen++] = op->cmd.opcode;

	if (op->addr.nbytes)
		for (i = 0; i < op->addr.nbytes; i++)
			txbuf[txlen++] = op->addr.val >>
					(8 * (op->addr.nbytes - i - 1));

	txlen += op->dummy.nbytes;

	if (op->data.dir == SPI_MEM_DATA_OUT)
		for (i = 0; i < op->data.nbytes; i++) {
			buf = (u8 *)op->data.buf.out;
			txbuf[txlen++] = buf[i];
		}

	if (op->data.dir == SPI_MEM_DATA_IN) {
		rxbuf = (u8 *)op->data.buf.in;
		rxlen += op->data.nbytes;
	}

	ret = mtk_snfi_command_exec(snfi, txbuf, rxbuf, txlen, rxlen);
	kfree(txbuf);

	return ret;
}

static int mtk_snfi_init(struct mtk_snfi *snfi)
{
	int ret;

	/* Reset the state machine and data FIFO */
	ret = mtk_snfi_reset(snfi);
	if (ret) {
		dev_warn(snfi->dev, "MTK reset controller fail\n");
		return ret;
	}

	snfi->buffer = devm_kzalloc(snfi->dev, 4096 + 256, GFP_KERNEL);
	if (!snfi->buffer)
		return  -ENOMEM;

	/* Clear interrupt, read clear. */
	readw(snfi->regs + NFI_INTR_STA);
	writew(0, snfi->regs + NFI_INTR_EN);

	writel(0, snfi->regs + NFI_CON);
	writel(0, snfi->regs + NFI_CNFG);

	/* Change to NFI_SPI mode. */
	writel(SNFI_MODE_EN, snfi->regs + SNFI_CNFG);

	return 0;
}

static int mtk_snfi_check_buswidth(u8 width)
{
	switch (width) {
	case 1:
	case 2:
	case 4:
		return 0;

	default:
		break;
	}

	return -ENOTSUPP;
}

static bool mtk_snfi_supports_op(struct spi_mem *mem,
				 const struct spi_mem_op *op)
{
	int ret = 0;

	/* For MTK Spi Nand controller, cmd buswidth just support 1 bit*/
	if (op->cmd.buswidth != 1)
		ret = -ENOTSUPP;

	if (op->addr.nbytes)
		ret |= mtk_snfi_check_buswidth(op->addr.buswidth);

	if (op->dummy.nbytes)
		ret |= mtk_snfi_check_buswidth(op->dummy.buswidth);

	if (op->data.nbytes)
		ret |= mtk_snfi_check_buswidth(op->data.buswidth);

	if (ret)
		return false;

	return true;
}

static const struct spi_controller_mem_ops mtk_snfi_ops = {
	.supports_op = mtk_snfi_supports_op,
	.exec_op = mtk_snfi_exec_op,
};

static const struct mtk_snfi_caps snfi_mt7622 = {
	.spare_size = spare_size_mt7622,
	.num_spare_size = 4,
	.nand_sec_size = 512,
	.nand_fdm_size = 8,
	.nand_fdm_ecc_size = 1,
	.ecc_parity_bits = 13,
	.pageformat_spare_shift = 4,
	.bad_mark_swap = 0,
};

static const struct mtk_snfi_caps snfi_mt7629 = {
	.spare_size = spare_size_mt7622,
	.num_spare_size = 4,
	.nand_sec_size = 512,
	.nand_fdm_size = 8,
	.nand_fdm_ecc_size = 1,
	.ecc_parity_bits = 13,
	.pageformat_spare_shift = 4,
	.bad_mark_swap = 1,
};

static const struct of_device_id mtk_snfi_id_table[] = {
	{ .compatible = "mediatek,mt7622-snfi", .data = &snfi_mt7622, },
	{ .compatible = "mediatek,mt7629-snfi", .data = &snfi_mt7629, },
	{  /* sentinel */ }
};

static int mtk_snfi_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct spi_controller *ctlr;
	struct mtk_snfi *snfi;
	struct resource *res;
	int ret = 0, irq;

	ctlr = spi_alloc_master(&pdev->dev, sizeof(*snfi));
	if (!ctlr)
		return -ENOMEM;

	snfi = spi_controller_get_devdata(ctlr);
	snfi->caps = of_device_get_match_data(dev);
	snfi->dev = dev;

	snfi->ecc = of_mtk_ecc_get(np);
	if (IS_ERR_OR_NULL(snfi->ecc))
		goto err_put_master;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	snfi->regs = devm_ioremap_resource(dev, res);
	if (IS_ERR(snfi->regs)) {
		ret = PTR_ERR(snfi->regs);
		goto release_ecc;
	}

	/* find the clocks */
	snfi->clk.nfi_clk = devm_clk_get(dev, "nfi_clk");
	if (IS_ERR(snfi->clk.nfi_clk)) {
		dev_err(dev, "no nfi clk\n");
		ret = PTR_ERR(snfi->clk.nfi_clk);
		goto release_ecc;
	}

	snfi->clk.spi_clk = devm_clk_get(dev, "spi_clk");
	if (IS_ERR(snfi->clk.spi_clk)) {
		dev_err(dev, "no spi clk\n");
		ret = PTR_ERR(snfi->clk.spi_clk);
		goto release_ecc;
	}

	ret = mtk_snfi_enable_clk(dev, &snfi->clk);
	if (ret)
		goto release_ecc;

	/* find the irq */
	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(dev, "no snfi irq resource\n");
		ret = -EINVAL;
		goto clk_disable;
	}

	ret = devm_request_irq(dev, irq, mtk_snfi_irq, 0, "mtk-snfi", snfi);
	if (ret) {
		dev_err(dev, "failed to request snfi irq\n");
		goto clk_disable;
	}

	ret = dma_set_mask(dev, DMA_BIT_MASK(32));
	if (ret) {
		dev_err(dev, "failed to set dma mask\n");
		goto clk_disable;
	}

	ctlr->dev.of_node = np;
	ctlr->mem_ops = &mtk_snfi_ops;

	platform_set_drvdata(pdev, snfi);
	ret = mtk_snfi_init(snfi);
	if (ret) {
		dev_err(dev, "failed to init snfi\n");
		goto clk_disable;
	}

	ret = devm_spi_register_master(dev, ctlr);
	if (ret)
		goto clk_disable;

	return 0;

clk_disable:
	mtk_snfi_disable_clk(&snfi->clk);

release_ecc:
	mtk_ecc_release(snfi->ecc);

err_put_master:
	spi_master_put(ctlr);

	dev_err(dev, "MediaTek SPI NAND interface probe failed %d\n", ret);
	return ret;
}

static int mtk_snfi_remove(struct platform_device *pdev)
{
	struct mtk_snfi *snfi = platform_get_drvdata(pdev);

	mtk_snfi_disable_clk(&snfi->clk);

	return 0;
}

static int mtk_snfi_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct mtk_snfi *snfi = platform_get_drvdata(pdev);

	mtk_snfi_disable_clk(&snfi->clk);

	return 0;
}

static int mtk_snfi_resume(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct mtk_snfi *snfi = dev_get_drvdata(dev);
	int ret;

	ret = mtk_snfi_enable_clk(dev, &snfi->clk);
	if (ret)
		return ret;

	ret = mtk_snfi_init(snfi);
	if (ret)
		dev_err(dev, "failed to init snfi controller\n");

	return ret;
}

static struct platform_driver mtk_snfi_driver = {
	.driver = {
		.name	= "mtk-snfi",
		.of_match_table = mtk_snfi_id_table,
	},
	.probe		= mtk_snfi_probe,
	.remove		= mtk_snfi_remove,
	.suspend	= mtk_snfi_suspend,
	.resume		= mtk_snfi_resume,
};

module_platform_driver(mtk_snfi_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Xiangsheng Hou <xiangsheng.hou@mediatek.com>");
MODULE_DESCRIPTION("Mediatek SPI Memory Interface Driver");
