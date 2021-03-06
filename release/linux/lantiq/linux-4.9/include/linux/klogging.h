#ifndef KLOGGING_H
#define KLOGGING_H
#include <linux/printk.h>
#include <linux/ratelimit.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/netdevice.h>

#define KER_LOG_EMERG 0
#define KER_LOG_ALERT 1
#define KER_LOG_CRIT 2
#define KER_LOG_ERR 3
#define KER_LOG_WARNING 4
#define KER_LOG_NOTICE 5
#define KER_LOG_INFO 6
#define KER_LOG_DEBUG 7

#undef pr_fmt

#define pr_fmt(fmt) "[" KBUILD_MODNAME "] { %s : %d }" fmt, __func__, __LINE__

#define LOGF_KLOG_EMERG(fmt, args...)		pr_emerg(fmt, ##args)
#define LOGF_KLOG_ALERT(fmt, args...)		pr_alert(fmt, ##args)
#define LOGF_KLOG_CRITICAL(fmt, args...)	pr_crit(fmt, ##args)
#define LOGF_KLOG_ERROR(fmt, args...)		pr_err(fmt, ##args)
#define LOGF_KLOG_WARNING(fmt, args...)		pr_warn(fmt, ##args)
#define LOGF_KLOG_NOTICE(fmt, args...)		pr_notice(fmt, ##args)
#define LOGF_KLOG_INFO(fmt, args...)		pr_info(fmt, ##args)
#define LOGF_KLOG_DEBUG(fmt, args...)		pr_debug(fmt, ##args)
#define LOGF_KLOG_CONT(fmt, args...)		printk(fmt, ##args)
#define LOGF_KLOG_DEVEL(fmt, args...)		pr_devel(fmt, ##args)
#define LOGF_KLOG_ONCE(fmt, args...)		printk_once(fmt, ##args)
#define LOGF_KLOG_EMERG_ONCE(fmt, args...)	pr_emerg_once(fmt, ##args)
#define LOGF_KLOG_ALERT_ONCE(fmt, args...)	pr_alert_once(fmt, ##args)
#define LOGF_KLOG_CRIT_ONCE(fmt, args...)	pr_crit_once(fmt, ##args)
#define LOGF_KLOG_ERR_ONCE(fmt, args...)	pr_err_once(fmt, ##args)
#define LOGF_KLOG_NOTICE_ONCE(fmt, args...)	pr_notice_once(fmt, ##args)
#define LOGF_KLOG_WARN_ONCE(fmt, args...)	pr_warn_once(fmt, ##args)
#define LOGF_KLOG_INFO_ONCE(fmt, args...)	pr_info_once(fmt, ##args)
#define LOGF_KLOG_CONT_ONCE(fmt, args...)	pr_cont_once(fmt, ##args)
#define LOGF_KLOG_DEVEL_ONCE(fmt, args...)	pr_devel_once(fmt, ##args)
#define LOGF_KLOG_DEBUG_ONCE(fmt, args...)	pr_debug_once(fmt, ##args)

#define LOGF_KLOG_RATELIMITED(fmt, args...)  printk_ratelimited(fmt, ##args)

#define LOGF_KLOG_ERR_RATELIMITED(fmt, args...) \
	pr_err_ratelimited(fmt, ##args)

#define LOGF_KLOG_ALERT_RATELIMITED(fmt, args...) \
	pr_alert_ratelimited(fmt, ##args)

#define LOGF_KLOG_CRIT_RATELIMITED(fmt, args...) \
	pr_crit_ratelimited(fmt, ##args)

#define LOGF_KLOG_WARN_RATELIMITED(fmt, args...) \
	pr_warn_ratelimited(fmt, ##args)

#define LOGF_KLOG_NOTICE_RATELIMITED(fmt, args...) \
	pr_notice_ratelimited(fmt, ##args)

#define LOGF_KLOG_INFO_RATELIMITED(fmt, args...) \
	pr_info_ratelimited(fmt, ##args)

#define LOGF_KLOG_CONT_RATELIMITED(fmt, args...) \
	printk(fmt, ##args)

#define LOGF_KLOG_DEVEL_RATELIMITED(fmt, args...) \
	pr_devel_ratelimited(fmt, ##args)

#define LOGF_KLOG_DEBUG_RATELIMITED(fmt, args...) \
	pr_debug_ratelimited(fmt, ##args)

#define LOGF_KLOG_DEV_DBG(dev , fmt, args...)	dev_dbg(dev , fmt , ##args)

#define LOGF_KLOG_DEV_EMERG(dev , fmt, args...)	\
	dev_emerg(dev , fmt , ##args)

#define LOGF_KLOG_DEV_ALERT(dev , fmt, args...)	\
	dev_alert(dev , fmt , ##args)

#define LOGF_KLOG_DEV_CRITICAL(dev , fmt, args...)	\
	dev_crit(dev , fmt , ##args)

#define LOGF_KLOG_DEV_ERROR(dev , fmt, args...)	\
	dev_err(dev , fmt , ##args)

#define LOGF_KLOG_DEV_WARNING(dev , fmt, args...)	\
	dev_warn(dev , fmt , ##args)

#define LOGF_KLOG_DEV_NOTICE(dev , fmt, args...)	\
	 dev_notice(dev , fmt , ##args)

#define LOGF_KLOG_DEV_INFO(dev , fmt, args...)	\
	dev_info(dev , fmt , ##args)

#define LOGF_KLOG_NET_DEV_DBG(dev , fmt, args...) \
	netdev_dbg(dev, fmt, ##args)

#define LOGF_KLOG_NET_DEV_EMERG(dev , fmt, args...) \
	netdev_printk(KERN_EMERG , dev , fmt , ##args)

#define LOGF_KLOG_NET_DEV_ALERT(dev , fmt, args...) \
	netdev_printk(KERN_ALERT , dev , fmt , ##args)

#define LOGF_KLOG_NET_DEV_CRITICAL(dev , fmt, args...) \
	netdev_printk(KERN_CRIT , dev , fmt , ##args)

#define LOGF_KLOG_NET_DEV_ERROR(dev , fmt, args...) \
	netdev_printk(KERN_ERR , dev , fmt , ##args)

#define LOGF_KLOG_NET_DEV_WARNING(dev , fmt, args...) \
	netdev_printk(KERN_WARNING , dev , fmt , ##args)

#define LOGF_KLOG_NET_DEV_NOTICE(dev , fmt, args...) \
	netdev_printk(KERN_NOTICE , dev , fmt , ##args)

#define LOGF_KLOG_NET_DEV_INFO(dev , fmt, args...) \
	netdev_printk(KERN_INFO, dev , fmt , ##args)

#define LOGF_KLOG_DEV_DBG_RATELIMITED(dev , fmt, args...) \
	pr_dbg_ratelimited(dev , fmt , ##args)

#define LOGF_KLOG_NETIF_DBG(priv, type, dev, fmt, args...) \
	netif_dbg(priv, type, dev, fmt, ##args)

#endif
