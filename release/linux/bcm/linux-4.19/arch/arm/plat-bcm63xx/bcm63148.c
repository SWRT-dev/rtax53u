#if defined(CONFIG_BCM_KF_ARM_BCM963XX)
/*
<:copyright-BRCM:2013:DUAL/GPL:standard

   Copyright (c) 2013 Broadcom 
   All Rights Reserved

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation (the "GPL").

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.


A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

:>
*/

/*
 * BCM63148 SoC main platform file.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/clk.h>
#include <linux/stop_machine.h>
#include <asm/mach/map.h>
#include <asm/system_misc.h>
#include <mach/hardware.h>
#include <mach/smp.h>
#include <plat/bsp.h>
#include <plat/b15core.h>
#include <bcm_map_part.h>
#include <bcm_intr.h>

#define FREQ_MHZ(x)	((x)*1000*1000)

#define IO_DESC(pa, sz) { \
		.virtual = IO_ADDRESS(pa), \
		.pfn = __phys_to_pfn(pa), \
		.length = sz, \
		.type = MT_DEVICE, \
	}

#define MEM_DESC(pa, sz) { \
		.virtual = IO_ADDRESS(pa), \
		.pfn = __phys_to_pfn(pa), \
		.length = sz, \
		.type = MT_MEMORY_RWX_NONCACHED, \
	}


static struct map_desc bcm63148_io_desc[] __initdata = {
	IO_DESC(USB_CTL_PHYS_BASE, SZ_4K),
	IO_DESC(MEMC_PHYS_BASE, SZ_4K),
	IO_DESC(DDRPHY_PHYS_BASE, SZ_4K),
	IO_DESC(SAR_PHYS_BASE, SZ_16K),
	IO_DESC(SATA_PHYS_BASE, SZ_16K),
	IO_DESC(USBH_PHYS_BASE, SZ_8K),
	IO_DESC(ERROR_PORT_PHYS_BASE, SZ_4K),
//	IO_DESC(L2C_PHYS_BASE, SZ_4K),
	IO_DESC(B15_CTRL_PHYS_BASE, SZ_16K),	// FIXME! once we know the real RDB
	IO_DESC(DECT_PHYS_BASE, SZ_128K),
	IO_DESC(SWITCH_PHYS_BASE, SZ_512K),
	IO_DESC(APM_PHYS_BASE, SZ_128K),
	IO_DESC(RDP_PHYS_BASE, SZ_1M),
	IO_DESC(PMC_PHYS_BASE, SZ_512K),
	IO_DESC(PROC_MON_PHYS_BASE, SZ_4K),
	IO_DESC(DSLPHY_PHYS_BASE, SZ_1M),
	IO_DESC(DSLLMEM_PHYS_BASE, SZ_1M),
	IO_DESC(PERF_PHYS_BASE, SZ_16K),
	IO_DESC(BOOTLUT_PHYS_BASE, SZ_4K),
	IO_DESC(SPIFLASH_PHYS_BASE, SZ_128K),
	IO_DESC(NANDFLASH_PHYS_BASE, SZ_128K),
	/* FIXME!! more!! */
};

/* any fixup that has to be performed in the early stage of
 * kernel booting */
void __init soc_fixup(void)
{
	b15_fixup();
}

/*
 * Map fix-mapped I/O that is needed before full MMU operation
 */
void __init soc_map_io(void)
{
	b15_map_io();

	iotable_init(bcm63148_io_desc, ARRAY_SIZE(bcm63148_io_desc));
}

static int arm_wfi_allowed = 1; // administratively allowed

/* power is significantly reduced by re-enabling interrupts
 * and looping locally until a reschedule is needed.
 * nops would help further but create droops/spikes.
 */
__attribute__ (( aligned(16),hot ))
static void bcm63xx_arm_pm_idle(void)
{
	local_irq_enable();
	while (!need_resched());
}

// selective wfi enable/disable based on frequency
void arm_wfi_enable(unsigned int freqHz)
{
	/* enable only if administratively allowed and under 1500MHz */
	if (arm_wfi_allowed && freqHz < FREQ_MHZ(1500)) {
		arm_pm_idle = 0;
	} else {
		arm_pm_idle = bcm63xx_arm_pm_idle;
	}
}

static unsigned int get_arm_core_clk(void)
{
	struct device *cpu_dev = NULL;
	struct clk *arm_clk = NULL;
	unsigned int clk_rate;

	cpu_dev = get_cpu_device(smp_processor_id());
	if (!cpu_dev) {
		return FREQ_MHZ(1500);
	}

	arm_clk = clk_get(cpu_dev, "cpuclk");
	if( IS_ERR_OR_NULL(arm_clk) ) {
	 	pr_err("%s: failed to get cpu clk\n", __func__);
		return FREQ_MHZ(1500);
	}

	clk_rate =  clk_get_rate(arm_clk);

	return clk_rate;
}

#if 0
static int soc_abort_handler(unsigned long addr, unsigned int fsr,
		struct pt_regs *regs)
{
	/*
	 * These happen for no good reason
	 * possibly left over from CFE
	 */
	printk(KERN_WARNING "External imprecise Data abort at "
			"addr=%#lx, fsr=%#x ignored.\n", addr, fsr);

	/* Returning non-zero causes fault display and panic */
	return 0;
}
#endif

static void soc_aborts_enable(void)
{
#if 0
	u32 x;

	/* Install our hook */
	hook_fault_code(16 + 6, soc_abort_handler, SIGBUS, 0,
			"imprecise external abort");

	/* Enable external aborts - clear "A" bit in CPSR */

	/* Read CPSR */
	asm( "mrs	%0,cpsr": "=&r" (x) : : );

	x &= ~ PSR_A_BIT;

	/* Update CPSR, affect bits 8-15 */
	asm( "msr	cpsr_x,%0; nop; nop": : "r" (x) : "cc" );
#endif
}

/*
 * This SoC relies on MPCORE GIC interrupt controller
 */
void __init soc_init_irq(void)
{
	b15_init_gic();
	soc_aborts_enable();
}

#ifdef CONFIG_CACHE_L2X0
/* 63148 integrate l2 cache controller, no need to init */
void  __init soc_l2_cache_init(void)
{
	return;
}
#endif

/*
 * SoC initialization that need to be done early,
 * e.g. L2 cache, clock, I/O pin mux, power management
 */
void __init soc_init_early(void)
{
	b15_init_early();

	/*
	 * DMA memory
	 *
	 * The PCIe-to-AXI mapping (PAX) has a window of 128 MB alighed at 1MB
	 * we should make the DMA-able DRAM at least this large.
	 * Will need to use CONSISTENT_BASE and CONSISTENT_SIZE macros
	 * to program the PAX inbound mapping registers.
	 */
	// FIXME!!!
	//init_consistent_dma_size(SZ_128M);
}

/*
 * Install all other SoC device drivers
 * that are not automatically discoverable.
 */

void __init soc_add_devices(void)
{
	/* if there is soc specific device */

	/* to ensure RAC is disabled, due to some known issues with RAC */
	B15CTRL->cpu_ctrl.rac_cfg0 = 0;
}

/*
 * Functions to allow enabling/disabling WAIT instruction
 */
void set_cpu_arm_wait(int enable)
{
	arm_wfi_allowed = enable;
	printk("wait instruction: %s\n", enable ? "enabled" : "disabled");
	arm_wfi_enable(get_arm_core_clk());
	kick_all_cpus_sync();
}
EXPORT_SYMBOL(set_cpu_arm_wait);

int get_cpu_arm_wait(void)
{
	return arm_wfi_allowed;
}
EXPORT_SYMBOL(get_cpu_arm_wait);

static int __init bcm963xx_idle_init(void)
{
	arm_wfi_enable(get_arm_core_clk());
	return 0;
}
arch_initcall(bcm963xx_idle_init);
#endif /* CONFIG_BCM_KF_ARM_BCM963XX */
