/*
 * drivers/i2c/busses/i2c-mt7621.c
 *
 * Copyright (C) 2013 Steven Liu <steven_liu@mediatek.com>
 * Copyright (C) 2016 Michael Lee <igvtee@gmail.com>
 *
 * Improve driver for i2cdetect from i2c-tools to detect i2c devices on the bus.
 * (C) 2014 Sittisak <sittisaks@hotmail.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/reset.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/clk.h>

#define REG_SM0CFG0		0x08
#define REG_SM0DOUT		0x10
#define REG_SM0DIN		0x14
#define REG_SM0ST		0x18
#define REG_SM0AUTO		0x1C
#define REG_SM0CFG1		0x20
#define REG_SM0CFG2		0x28
#define REG_SM0CTL0		0x40
#define REG_SM0CTL1		0x44
#define REG_SM0D0		0x50
#define REG_SM0D1		0x54
#define REG_PINTEN		0x5C
#define REG_PINTST		0x60
#define REG_PINTCL		0x64

//#define I2C_MANUAL_MODE

#ifdef I2C_MANUAL_MODE
#define MM_I2C_REG_BASE		0x0
#define MM_I2C_CONFIG_REG	(MM_I2C_REG_BASE + 0x00)
#define MM_I2C_CLKDIV_REG	(MM_I2C_REG_BASE + 0x04)
#define MM_I2C_DEVADDR_REG	(MM_I2C_REG_BASE + 0x08)
#define MM_I2C_ADDR_REG		(MM_I2C_REG_BASE + 0x0C)
#define MM_I2C_DATAOUT_REG	(MM_I2C_REG_BASE + 0x10)
#define MM_I2C_DATAIN_REG	(MM_I2C_REG_BASE + 0x14)
#define MM_I2C_STATUS_REG	(MM_I2C_REG_BASE + 0x18)
#define MM_I2C_STARTXFR_REG	(MM_I2C_REG_BASE + 0x1C)
#define MM_I2C_BYTECNT_REG	(MM_I2C_REG_BASE + 0x20)
#define MM_I2C_AUTOMODE_REG	(MM_I2C_REG_BASE + 0x28)
#define MM_I2C_SM0CTL0_REG	(MM_I2C_REG_BASE + 0x40)
#define MM_I2C_SM0CTL1_REG	(MM_I2C_REG_BASE + 0x44)
#define MM_I2C_SM0D0_REG	(MM_I2C_REG_BASE + 0x50)
#define MM_I2C_SM0D1_REG	(MM_I2C_REG_BASE + 0x54)
/* I2C_SM0_CTL register bit field */
#define MM_SM0_ODRAIN_PULL	(0x1 << 31)			/* The output is pulled hight by SIF master 0 */
#define MM_SM0_VSYNC_PULSE	(0x1 << 28)			/* Allow triggered in VSYNC pulse */
#define MM_SM0_WAIT_H		(0x1 << 6)			/* Output H when SIF master 0 is in WAIT state */
#define MM_SM0_EN		(0x1 << 1)			/* Enable SIF master 0 */
/* I2C_SM1_CTL register bit field */
#define MM_SM0_ACK		(0xff << 16)			/* Acknowledge of 8 bytes of data */
#define MM_SM0_TRI		(0x1 << 0)			/* Trigger serial interface */
#define MM_SM0_MODE_START	((0x1 << 4) | MM_SM0_TRI)	/* SIF master mode start */
#define MM_SM0_MODE_WRITE_DATA	((0x2 << 4) | MM_SM0_TRI)	/* SIF master mode write data */
#define MM_SM0_MODE_STOP	((0x3 << 4) | MM_SM0_TRI)	/* SIF master mode stop */
#define MM_SM0_MODE_READ_NACK 	((0x4 << 4) | MM_SM0_TRI)	/* SIF master mode read data with no ack for final byte */
#define MM_SM0_MODE_READ_ACK	((0x5 << 4) | MM_SM0_TRI)	/* SIF master mode read data with ack */
#define MM_SM0_PGLEN(x)		((x) << 8)			/* Page length */
#define MM_SM0_MAX_PGLEN	(0x7 << 8)			/* Max page length */
/* I2C_CFG register bit field */
#define MM_CFG_ADDRLEN_8	(0x7 << 5)			/* 8 bits */
#define MM_CFG_DEVADLEN_7	(0x6 << 2)			/* 7 bits */
#define MM_CFG_ADDRDIS		(0x1 << 1)			/* Disable address transmission*/
#define MM_CFG_DEFAULT		(MM_CFG_ADDRLEN_8 | MM_CFG_DEVADLEN_7 | MM_CFG_ADDRDIS)
/* register access command */
#define MM_REG_CMD_W(_i2c, _reg, _val)	mtk_i2c_w32(_i2c, _val, _reg)
#define MM_REG_CMD_R(_i2c, _reg)	mtk_i2c_r32(_i2c, _reg)
/* common variable */
/* source clock */
#define MM_PERI_CLK		50000
#define MM_MAX_MODE_SPEED	500	/* khz */
static int mm_delay_timing = MM_PERI_CLK / (MM_MAX_MODE_SPEED * 2);
#define MM_DATARDY		0x04
#define MM_SDOEMPTY		0x02
#define MM_DATABUSY		0x01
#define MM_READ_BLOCK		16
#define MM_MAX_SIZE		64
/* Serial interface master data max byte count  */
#define MM_SM0_DATA_COUNT	4
/* Serial data shift */
#define MM_SM0_DATA_SHIFT(x)	((x & 0x3) << 3)
/* controller speed limitation*/
#define MM_ST_MODE_SPEED	100	/* khz */
#define MM_FS_MODE_SPEED	400	/* khz */
#define MM_MAX_MODE_SPEED	500	/* khz */
/* Instruction codes */
#define MM_READ_CMD		0x1
#define MM_WRITE_CMD		0x0
#endif /* I2C_MANUAL_MODE */

/* REG_SM0CFG0 */
#define I2C_DEVADDR_MASK	0x7f

/* REG_SM0ST */
#define I2C_DATARDY		BIT(2)
#define I2C_SDOEMPTY		BIT(1)
#define I2C_BUSY		BIT(0)

/* REG_SM0AUTO */
#define READ_CMD		BIT(0)

/* REG_SM0CFG1 */
#define BYTECNT_MAX		64
#define SET_BYTECNT(x)		(x - 1)

/* REG_SM0CFG2 */
#define AUTOMODE_EN		BIT(0)

/* REG_SM0CTL0 */
#define ODRAIN_HIGH_SM0		BIT(31)
#define VSYNC_SHIFT		28
#define VSYNC_MASK		0x3
#define VSYNC_PULSE		(0x1 << VSYNC_SHIFT)
#define VSYNC_RISING		(0x2 << VSYNC_SHIFT)
#define CLK_DIV_SHIFT		16
#define CLK_DIV_MASK		0xfff
#define DEG_CNT_SHIFT		8
#define DEG_CNT_MASK		0xff
#define WAIT_HIGH		BIT(6)
#define DEG_EN			BIT(5)
#define CS_STATUA		BIT(4)
#define SCL_STATUS		BIT(3)
#define SDA_STATUS		BIT(2)
#define SM0_EN			BIT(1)
#define SCL_STRECH		BIT(0)

/* REG_SM0CTL1 */
#define ACK_SHIFT		16
#define ACK_MASK		0xff
#define PGLEN_SHIFT		8
#define PGLEN_MASK		0x7
#define SM0_MODE_SHIFT		4
#define SM0_MODE_MASK		0x7
#define SM0_MODE_START		0x1
#define SM0_MODE_WRITE		0x2
#define SM0_MODE_STOP		0x3
#define SM0_MODE_READ_NACK	0x4
#define SM0_MODE_READ_ACK	0x5
#define SM0_TRI_BUSY		BIT(0)

/* timeout waiting for I2C devices to respond (clock streching) */
#define TIMEOUT_MS              1000
#define DELAY_INTERVAL_US       100

struct mtk_i2c {
	void __iomem *base;
	struct clk *clk;
	struct device *dev;
	struct i2c_adapter adap;
	u32 cur_clk;
	u32 clk_div;
	u32 flags;
};

static void mtk_i2c_w32(struct mtk_i2c *i2c, u32 val, unsigned reg)
{
	iowrite32(val, i2c->base + reg);
}

static u32 mtk_i2c_r32(struct mtk_i2c *i2c, unsigned reg)
{
	return ioread32(i2c->base + reg);
}

static int poll_down_timeout(void __iomem *addr, u32 mask)
{
	unsigned long timeout = jiffies + msecs_to_jiffies(TIMEOUT_MS);

	do {
		if (!(readl_relaxed(addr) & mask))
			return 0;

		usleep_range(DELAY_INTERVAL_US, DELAY_INTERVAL_US + 50);
	} while (time_before(jiffies, timeout));

	return (readl_relaxed(addr) & mask) ? -EAGAIN : 0;
}

static int mtk_i2c_wait_idle(struct mtk_i2c *i2c)
{
	int ret;

	ret = poll_down_timeout(i2c->base + REG_SM0ST, I2C_BUSY);
	if (ret < 0)
		dev_dbg(i2c->dev, "idle err(%d)\n", ret);

	return ret;
}

static int poll_up_timeout(void __iomem *addr, u32 mask)
{
	unsigned long timeout = jiffies + msecs_to_jiffies(TIMEOUT_MS);
	u32 status;

	do {
		status = readl_relaxed(addr);
		if (status & mask)
			return 0;
		usleep_range(DELAY_INTERVAL_US, DELAY_INTERVAL_US + 50);
	} while (time_before(jiffies, timeout));

	return -ETIMEDOUT;
}

static int mtk_i2c_wait_rx_done(struct mtk_i2c *i2c)
{
	int ret;

	ret = poll_up_timeout(i2c->base + REG_SM0ST, I2C_DATARDY);
	if (ret < 0)
		dev_dbg(i2c->dev, "rx err(%d)\n", ret);

	return ret;
}

static int mtk_i2c_wait_tx_done(struct mtk_i2c *i2c)
{
	int ret;

	ret = poll_up_timeout(i2c->base + REG_SM0ST, I2C_SDOEMPTY);
	if (ret < 0)
		dev_dbg(i2c->dev, "tx err(%d)\n", ret);

	return ret;
}

static void mtk_i2c_reset(struct mtk_i2c *i2c)
{
	u32 reg;

	device_reset(i2c->adap.dev.parent);
	barrier();

	/* ctrl0 */
	reg = ODRAIN_HIGH_SM0 | VSYNC_PULSE | (i2c->clk_div << CLK_DIV_SHIFT) |
		WAIT_HIGH | SM0_EN;
	mtk_i2c_w32(i2c, reg, REG_SM0CTL0);

#ifdef I2C_MANUAL_MODE
	/* manual mode */
	mtk_i2c_w32(i2c, 0x0, REG_SM0CFG2);
#else
	/* auto mode */
	mtk_i2c_w32(i2c, AUTOMODE_EN, REG_SM0CFG2);
#endif /* I2C_MANUAL_MODE */
}

static void mtk_i2c_dump_reg(struct mtk_i2c *i2c)
{
	dev_dbg(i2c->dev, "cfg0 %08x, dout %08x, din %08x\n",
			mtk_i2c_r32(i2c, REG_SM0CFG0),
			mtk_i2c_r32(i2c, REG_SM0DOUT),
			mtk_i2c_r32(i2c, REG_SM0DIN));

	dev_dbg(i2c->dev, "status %08x, auto %08x, cfg1 %08x\n",
			mtk_i2c_r32(i2c, REG_SM0ST),
			mtk_i2c_r32(i2c, REG_SM0AUTO),
			mtk_i2c_r32(i2c, REG_SM0CFG1));

	dev_dbg(i2c->dev, "cfg2 %08x, ctl0 %08x, ctl1 %08x\n",
			mtk_i2c_r32(i2c, REG_SM0CFG2),
			mtk_i2c_r32(i2c, REG_SM0CTL0),
			mtk_i2c_r32(i2c, REG_SM0CTL1));
}

#ifdef I2C_MANUAL_MODE
static void manual_mode_read(struct mtk_i2c *i2c, struct i2c_msg* msg)
{
	int i, cnt;
	u32 reg, data0, data1;

	/* re-start */
	MM_REG_CMD_W(i2c, MM_I2C_SM0CTL1_REG, MM_SM0_MODE_START);
	/* Write address for read */
	MM_REG_CMD_W(i2c, MM_I2C_SM0D0_REG, (msg->addr << 1) | MM_READ_CMD);
	//udelay(mm_delay_timing);
	udelay(100);

	MM_REG_CMD_W(i2c, MM_I2C_SM0CTL1_REG, MM_SM0_MODE_WRITE_DATA);
	//udelay(mm_delay_timing);
	udelay(100);
	while(!(MM_REG_CMD_R(i2c, MM_I2C_SM0CTL0_REG) & MM_SM0_ACK));

	for (cnt = 0; cnt < msg->len; cnt += (2 * MM_SM0_DATA_COUNT)) {
		if ((msg->len - cnt) > (2 * MM_SM0_DATA_COUNT))
			reg = MM_SM0_MODE_READ_ACK | MM_SM0_MAX_PGLEN;
		else
			reg = MM_SM0_MODE_READ_NACK | MM_SM0_PGLEN((msg->len - cnt) - 1);
		/* read data */
		MM_REG_CMD_W(i2c, MM_I2C_SM0CTL1_REG, reg);
		//udelay(mm_delay_timing);
		udelay(100);

		data0 = MM_REG_CMD_R(i2c, MM_I2C_SM0D0_REG);
		data1 = MM_REG_CMD_R(i2c, MM_I2C_SM0D1_REG);

		for (i = 0; i < (msg->len - cnt); i++) {
			if (i < MM_SM0_DATA_COUNT)
				msg->buf[cnt + i] = (data0 >> MM_SM0_DATA_SHIFT(i)) & 0xff;
			else
				msg->buf[cnt + i] = (data1 >> MM_SM0_DATA_SHIFT(i)) & 0xff;
		}
	}
	/* stop */
	MM_REG_CMD_W(i2c, MM_I2C_SM0CTL1_REG, MM_SM0_MODE_STOP);}

static void manual_mode_write(struct mtk_i2c *i2c, struct i2c_msg* msg, int num)
{
	int i, cnt;
	u32 data0, data1, reg;

	/* Start */
	MM_REG_CMD_W(i2c, MM_I2C_SM0CTL1_REG, MM_SM0_MODE_START);
	/* Write address for write */
	MM_REG_CMD_W(i2c, MM_I2C_SM0D0_REG, (msg->addr << 1) | MM_WRITE_CMD);
	//udelay(mm_delay_timing);
	udelay(100);

	MM_REG_CMD_W(i2c, MM_I2C_SM0CTL1_REG, MM_SM0_MODE_WRITE_DATA);
	//udelay(mm_delay_timing);
	udelay(100);
	/* wait slave ack */
	while(!(MM_REG_CMD_R(i2c, MM_I2C_SM0CTL0_REG) & MM_SM0_ACK));

	for (cnt = 0; cnt < msg->len; cnt += (2 * MM_SM0_DATA_COUNT)) {
		data0 = data1 = 0;
		for (i = 0; i < (msg->len - cnt); i++) {
			if (i < MM_SM0_DATA_COUNT)
				data0 |= msg->buf[cnt + i] << MM_SM0_DATA_SHIFT(i);
			else
				data1 |= msg->buf[cnt + i] << MM_SM0_DATA_SHIFT(i);
		}
		MM_REG_CMD_W(i2c, MM_I2C_SM0D0_REG, data0);
		MM_REG_CMD_W(i2c, MM_I2C_SM0D1_REG, data1);
		//udelay(mm_delay_timing);
		udelay(100);

		if ((msg->len - i) >= (2 * MM_SM0_DATA_COUNT))
			reg = MM_SM0_MODE_WRITE_DATA | MM_SM0_MAX_PGLEN;
		else
			reg = MM_SM0_MODE_WRITE_DATA | MM_SM0_PGLEN((msg->len - cnt) - 1);
		/* write data */
		MM_REG_CMD_W(i2c, MM_I2C_SM0CTL1_REG, reg);
		//udelay(mm_delay_timing);
		udelay(100);
		while(!(MM_REG_CMD_R(i2c, MM_I2C_SM0CTL0_REG) & MM_SM0_ACK));
	}

	/* Workaround #1: add delay between dummy-write and read */
	if ((msg + 1)->flags == I2C_M_RD && num >= 2) {
		return;
	}else{
		udelay(100);
	}

	/* Fix #0: ignore I2C_STOP action only when i2c msg is dummy-write  */
	if ((msg + 1)->flags == I2C_M_RD && num >= 2){
		return;
	}else {
		/* stop */
		MM_REG_CMD_W(i2c, MM_I2C_SM0CTL1_REG, MM_SM0_MODE_STOP);
	}
}
#endif /* I2C_MANUAL_MODE */

#ifdef I2C_MANUAL_MODE
static int mtk_i2c_master_mm_xfer(struct i2c_adapter *adap,
				  struct i2c_msg* msgs, int num)
{
	struct mtk_i2c *i2c;
	struct i2c_msg *pmsg;
	int left_num = num;
	int i;
	int ret;

	i2c = i2c_get_adapdata(adap);


	for (i = 0; i < num; i++) {
		pmsg = &msgs[i];
		if (pmsg->flags & I2C_M_TEN) {
			dev_dbg(i2c->dev, "10 bits addr not supported\n");
			return -EINVAL;
		}

		if (pmsg->len > MM_MAX_SIZE) {
			dev_dbg(i2c->dev, "please reduce data length\n");
			return -EINVAL;
		}

		/* buffer length */
		if (pmsg->len == 0) {
			dev_dbg(i2c->dev, "length is 0\n");
			return -EINVAL;
		}

		if (pmsg->flags & I2C_M_RD)
			manual_mode_read(i2c, pmsg);
		else
			manual_mode_write(i2c, pmsg, num);

	}
	/* Workaround #2 : add delay between i2c_master_xfer  */
	udelay(200);
	/* the return value is number of executed messages */
	ret = i;

	return ret;
}
#else
static int mtk_i2c_master_xfer(struct i2c_adapter *adap, struct i2c_msg *msgs,
		int num)
{
	struct mtk_i2c *i2c;
	struct i2c_msg *pmsg;
	int i, j, ret;
	u32 cmd;

	i2c = i2c_get_adapdata(adap);

	for (i = 0; i < num; i++) {
		pmsg = &msgs[i];
		cmd = 0;

		dev_dbg(i2c->dev, "addr: 0x%x, len: %d, flags: 0x%x\n",
				pmsg->addr, pmsg->len, pmsg->flags);

		/* wait hardware idle */
		ret = mtk_i2c_wait_idle(i2c);
		if (ret)
			goto err_timeout;

		if (pmsg->flags & I2C_M_TEN) {
			dev_dbg(i2c->dev, "10 bits addr not supported\n");
			return -EINVAL;
		}

		/* 7 bits address */
		mtk_i2c_w32(i2c, pmsg->addr & I2C_DEVADDR_MASK, REG_SM0CFG0);


		/* buffer length */
		if (pmsg->len == 0) {
			dev_dbg(i2c->dev, "length is 0\n");
			return -EINVAL;
		}

		mtk_i2c_w32(i2c, SET_BYTECNT(pmsg->len), REG_SM0CFG1);

		j = 0;
		if (pmsg->flags & I2C_M_RD) {
			cmd |= READ_CMD;
			/* start transfer */
			barrier();
			mtk_i2c_w32(i2c, cmd, REG_SM0AUTO);
			do {
				/* wait */
				ret = mtk_i2c_wait_rx_done(i2c);
				if (ret)
					goto err_timeout;
				/* read data */
				if (pmsg->len)
					pmsg->buf[j] = mtk_i2c_r32(i2c,
							REG_SM0DIN);
				j++;
			} while (j < pmsg->len);
		} else {
			do {
				/* write data */
				if (pmsg->len)
					mtk_i2c_w32(i2c, pmsg->buf[j],
							REG_SM0DOUT);
				/* start transfer */
				if (j == 0) {
					barrier();
					mtk_i2c_w32(i2c, cmd, REG_SM0AUTO);
				}
				/* wait */
				ret = mtk_i2c_wait_tx_done(i2c);
				if (ret)
					goto err_timeout;
				j++;
			} while (j < pmsg->len);
		}
	}
	/* the return value is number of executed messages */
	ret = i;

	return ret;

err_timeout:
	mtk_i2c_dump_reg(i2c);
	mtk_i2c_reset(i2c);
	return ret;
}
#endif /* I2C_MANUAL_MODE */

static u32 mtk_i2c_func(struct i2c_adapter *a)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

static const struct i2c_algorithm mtk_i2c_algo = {
#ifdef I2C_MANUAL_MODE
	.master_xfer	= mtk_i2c_master_mm_xfer,
#else
	.master_xfer	= mtk_i2c_master_xfer,
#endif
	.functionality	= mtk_i2c_func,
};

static const struct of_device_id i2c_mtk_dt_ids[] = {
	{ .compatible = "mediatek,mt7621-i2c" },
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, i2c_mtk_dt_ids);

static struct i2c_adapter_quirks mtk_i2c_quirks = {
	.max_write_len = BYTECNT_MAX,
	.max_read_len = BYTECNT_MAX,
};

static void mtk_i2c_init(struct mtk_i2c *i2c)
{
	i2c->clk_div = clk_get_rate(i2c->clk) / i2c->cur_clk;
	if (i2c->clk_div > CLK_DIV_MASK)
		i2c->clk_div = CLK_DIV_MASK;
	mtk_i2c_reset(i2c);
}

static int mtk_i2c_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct mtk_i2c *i2c;
	struct i2c_adapter *adap;
	const struct of_device_id *match;
	int ret;

	match = of_match_device(i2c_mtk_dt_ids, &pdev->dev);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENODEV;

	i2c = devm_kzalloc(&pdev->dev, sizeof(struct mtk_i2c), GFP_KERNEL);
	if (!i2c)
		return -ENOMEM;

	i2c->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(i2c->base))
		return PTR_ERR(i2c->base);

	i2c->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(i2c->clk)) {
		dev_notice(&pdev->dev, "no clock defined\n");
		return -ENODEV;
	}
	clk_prepare_enable(i2c->clk);
	i2c->dev = &pdev->dev;

	if (of_property_read_u32(pdev->dev.of_node,
				"clock-frequency", &i2c->cur_clk))
		i2c->cur_clk = 100000;

	adap = &i2c->adap;
	adap->owner = THIS_MODULE;
	adap->class = I2C_CLASS_HWMON | I2C_CLASS_SPD;
	adap->algo = &mtk_i2c_algo;
	adap->retries = 3;
	adap->dev.parent = &pdev->dev;
	i2c_set_adapdata(adap, i2c);
	adap->dev.of_node = pdev->dev.of_node;
	strlcpy(adap->name, dev_name(&pdev->dev), sizeof(adap->name));
	adap->quirks = &mtk_i2c_quirks;

	platform_set_drvdata(pdev, i2c);

	mtk_i2c_init(i2c);

	ret = i2c_add_adapter(adap);
	if (ret < 0) {
		dev_notice(&pdev->dev, "failed to add adapter\n");
		clk_disable_unprepare(i2c->clk);
		return ret;
	}

	dev_info(&pdev->dev, "clock %uKHz, re-start not support\n",
			i2c->cur_clk/1000);

	return ret;
}

static int mtk_i2c_remove(struct platform_device *pdev)
{
	struct mtk_i2c *i2c = platform_get_drvdata(pdev);

	i2c_del_adapter(&i2c->adap);
	clk_disable_unprepare(i2c->clk);

	return 0;
}

static struct platform_driver mtk_i2c_driver = {
	.probe		= mtk_i2c_probe,
	.remove		= mtk_i2c_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "i2c-mt7621",
		.of_match_table = i2c_mtk_dt_ids,
	},
};

static int __init i2c_mtk_init(void)
{
	return platform_driver_register(&mtk_i2c_driver);
}
subsys_initcall(i2c_mtk_init);

static void __exit i2c_mtk_exit(void)
{
	platform_driver_unregister(&mtk_i2c_driver);
}
module_exit(i2c_mtk_exit);

MODULE_AUTHOR("Steven Liu <steven_liu@mediatek.com>");
MODULE_DESCRIPTION("MT7621 I2c host driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:MT7621-I2C");
