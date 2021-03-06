/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <asm/gic.h>
#include <linux/bug.h>
#include <linux/clk.h>

#include <lantiq_timer.h>

/* RCU MACROs */
void __iomem *rcu_membase = (void *)0xb6000000;
#define RCU_IAP_WDT_RST_EN            0x0050
#define	RCU_WDTx_RESET		      0x100 			

#define TIMER_MARGIN	60	
	
static unsigned int timeout = TIMER_MARGIN;	/* in seconds */
module_param(timeout, uint, 0);
MODULE_PARM_DESC(timeout, 
	"Watchdog timeout in seconds "
        "(default=" __MODULE_STRING(TIMER_MARGIN) ")");

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout,
	"Watchdog cannot be stopped once started "
	"(default=" __MODULE_STRING(WATCHDOG_NOWAYOUT) ")");


static struct watchdog_device bootcorewdt;
int timeout_T2, timeout_T3, tdir;
static uint32_t bootcorewdt_get_timeleft(struct watchdog_device *wdt_dev);
static unsigned long bcpu_clk;

static int wdt_T2_callback (unsigned long param)
{

        WARN(1, " WDT Timer 2 Overflow param = %ld !!!.. \n", param);

        return IRQ_HANDLED;
}

static int bootcorewdt_start(struct watchdog_device *wdt_dev)
{
	int ret ;

       /* Request the GPTU Timer 2 and Timer 3 */
	ret = lq_request_timer(TIMER2A, TIMER_FLAG_32BIT|TIMER_FLAG_TIMER|TIMER_FLAG_ONCE|tdir|TIMER_FLAG_CALLBACK_IN_IRQ, (bcpu_clk * timeout_T2), (unsigned long)wdt_T2_callback, TIMER2A);
        if(unlikely(ret < 0)) {
                printk ("Request TIMER2A failed ..\n");
               	return ret; 
        }


	ret = lq_request_timer(TIMER3A, TIMER_FLAG_32BIT|TIMER_FLAG_TIMER|TIMER_FLAG_ONCE|tdir|TIMER_FLAG_CALLBACK_IN_IRQ, (bcpu_clk * (wdt_dev->timeout)), 0, 0);
        if(unlikely(ret < 0)) {
                printk ("Request TIMER3A failed ..\n");
		return ret;
        }

	ret = lq_start_timer(TIMER2A, 0);
	if (unlikely(ret < 0))
		return ret;
	
	ret = lq_start_timer(TIMER3A, 0);
	if (unlikely(ret < 0))
		return ret;

	//printk ("[%s] : [%d] \n", __FUNCTION__, __LINE__);
	return 0;
}

static int bootcorewdt_stop(struct watchdog_device *wdt_dev)
{
	int ret;
	ret = lq_stop_timer(TIMER3A);
        if (unlikely(ret < 0))
                return ret;

	ret = lq_stop_timer(TIMER2A);
        if (unlikely(ret < 0))
                return ret;

        /* Free TIMER3A and TIMER2A */
        lq_free_timer(TIMER3A);
        lq_free_timer(TIMER2A);

	//printk ("[%s] : [%d] \n", __FUNCTION__, __LINE__);

	return 0;
}

static int bootcorewdt_set_timeout(struct watchdog_device *wdt_dev,
			       unsigned int new_timeout)
{
	/* Stop the timers*/
	bootcorewdt_stop(wdt_dev);

	/* update the wdt structure and call lq_request_timer */
	wdt_dev->timeout = new_timeout;

	/* Start the timer */
	bootcorewdt_start(wdt_dev);

	printk("%s: timeout = %d, cpu = %d, id = %d \n", __func__, new_timeout, smp_processor_id(), wdt_dev->id);

	return 0;
}

static uint32_t bootcorewdt_get_timeleft(struct watchdog_device *wdt_dev)
{
	unsigned long count_T3, count_T2;

	lq_get_count_value(TIMER2A, &count_T2);
	lq_get_count_value(TIMER3A, &count_T3);
	
	// printk ("[%s] : [%d] T3 = %ld , T2 = %ld Time3 in sec = %d Time2 in sec = %d \n", __FUNCTION__, __LINE__, count_T3, count_T2, count_T3 / bcpu_clk, count_T2 / bcpu_clk);

        return (count_T3 / bcpu_clk);
}

static int bootcorewdt_ping(struct watchdog_device *wdt_dev)
{
	int ret;

	/* Stop the timers*/
	bootcorewdt_stop(wdt_dev);

	/* Start the Timers */
	bootcorewdt_start(wdt_dev);

	/* bootcorewdt_get_timeleft(wdt_dev); */

        return 0;
}

static long bootcorewdt_ioctl (struct watchdog_device *wdd , unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	int __user *p = argp;
	unsigned int pval;

	switch (cmd) {
	case WDIOC_SETPRETIMEOUT:

		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;
		if (get_user(pval, p))
			return -EFAULT;

		timeout_T2 = pval;
		bootcorewdt_ping(wdd);

	case WDIOC_GETPRETIMEOUT:
		if (timeout_T2 == 0)
			return -EOPNOTSUPP;
	
		return put_user(timeout_T2, p);

	default:
		return -ENOIOCTLCMD;
	}
}


static const struct watchdog_info bootcorewdt_info = {
        .options = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE | WDIOF_PRETIMEOUT,
        .identity = "Hardware Watchdog for BOOTCORE",
};

static const struct watchdog_ops bootcorewdt_ops = {
        .owner = THIS_MODULE,
        .start = bootcorewdt_start,
        .stop = bootcorewdt_stop,
        .ping = bootcorewdt_ping,
        .set_timeout = bootcorewdt_set_timeout,
        .get_timeleft = bootcorewdt_get_timeleft,
	.ioctl = bootcorewdt_ioctl,
};

static struct watchdog_device bootcorewdt = {
	.id = 0,
	.info = &bootcorewdt_info,
	.ops = &bootcorewdt_ops,
};

static int bootcorewdt_probe(struct platform_device *pdev)
{
	struct clk *clk;
	struct device_node *node = pdev->dev.of_node;

	timeout_T3 = timeout;

	if (of_property_read_u32(node, "timeout-sec-T2", &timeout_T2) != 0) {
		timeout_T2 = (timeout / 2);
	}
	
	if (of_property_read_u32(node, "timeout-sec-T3", &timeout_T3) != 0) {
		timeout_T3 = timeout;
	}

	WARN_ONCE((timeout_T2 > timeout_T3), " TIMER 3 Timeout is less than TIMER 2 Timeout !!! \n");

       /* Get the Clock frequency */
        clk = clk_get_sys("cpu", "cpu");
        if (clk == NULL) {
                pr_err("CPU clock structure not found\n");
                return -1;
        }

        bcpu_clk = clk_get_rate(clk);

	if (of_property_read_u32(node, "timer-dir", &tdir) != 0)
		tdir = 0; /* Count Down = 0 and Count up = 1 */	

	
	/* Enable WDT reset to RCU for VPEx */  
	__raw_writel(RCU_WDTx_RESET, rcu_membase + RCU_IAP_WDT_RST_EN);
	wmb();


	bootcorewdt.min_timeout = 1;
	bootcorewdt.max_timeout = (0xffffffff / bcpu_clk);

	watchdog_init_timeout(&bootcorewdt, timeout_T3 , &pdev->dev);
	watchdog_set_nowayout(&bootcorewdt, nowayout);

	watchdog_register_device(&bootcorewdt);

	pr_info("Bootcore Hardware Watchdog Timer Initialised !!! : (max %ld) (nowayout= %d)\n", (0xffffffff / bcpu_clk), nowayout);

	return 0;
}

static int __exit bootcorewdt_remove(struct platform_device *dev)
{
	bootcorewdt_stop(&bootcorewdt);
	watchdog_unregister_device(&bootcorewdt);

	return 0;
}

static void bootcorewdt_shutdown(struct platform_device *dev)
{
	bootcorewdt_stop(&bootcorewdt);
}

static const struct of_device_id bootcorewdt_match[] = {
        { .compatible = "lantiq,bootcorewdt" },
        {},
};

static struct platform_driver bootcorewdt_driver = {
	.probe = bootcorewdt_probe,
	.remove = __exit_p(bootcorewdt_remove),
	.shutdown = bootcorewdt_shutdown,
	.driver = {
		.name = "bootcorewdt",
		.owner = THIS_MODULE,
		.of_match_table = bootcorewdt_match,
	},
};

module_platform_driver(bootcorewdt_driver);

MODULE_DESCRIPTION("GRX500 BOOTCORE Watchdog Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:bootcorewdt");
