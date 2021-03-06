/*
 * Copyright (c) [2020], MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws.
 * The information contained herein is confidential and proprietary to
 * MediaTek Inc. and/or its licensors.
 * Except as otherwise provided in the applicable licensing terms with
 * MediaTek Inc. and/or its licensors, any reproduction, modification, use or
 * disclosure of MediaTek Software, and information contained herein, in whole
 * or in part, shall be strictly prohibited.
*/
/*
 ***************************************************************************
 ***************************************************************************

	Module Name:
	wifi_sys_notify.c
*/
#include "rt_config.h"
#include "wifi_sys_notify.h"

/*
*
*/
static INT raw_notify_chain_register(struct notify_entry **nl,
			struct notify_entry *n)
{
	while ((*nl) != NULL) {
		/*re-register*/
		if ((*nl) == n)
			return 0;
		if (n->priority > (*nl)->priority)
			break;
		nl = &((*nl)->next);
	}
	n->next = *nl;
	/*replace head to n*/
	*nl = n;
	return 0;
}

/*
*
*/
static INT raw_notify_chain_unregister(struct notify_entry **nl,
			struct notify_entry *n)
{
	while ((*nl) != NULL) {
		if ((*nl) == n) {
			*nl = n->next;
			return 0;
		}
		nl = &((*nl)->next);
	}
	return -1;
}

/*
*
*/
static INT raw_notify_call_chain(struct notify_entry **nl,
			INT val, VOID *v)
{
	INT ret = NOTIFY_STAT_DONE;
	struct notify_entry *entry;
	struct notify_entry *next_entry;

	entry = *nl;

	while (entry) {
		next_entry = entry->next;
		ret = entry->notify_call(entry, val, v);

		if (ret & NOTIFY_STAT_STOP)
			break;
		entry = next_entry;
	}
	return ret;
}

/*
*
*/
INT mt_notify_chain_register(struct notify_head *nh, struct notify_entry *entry)
{
	INT ret;

	OS_SPIN_LOCK(&nh->lock);
	ret = raw_notify_chain_register(&nh->head, entry);
	OS_SPIN_UNLOCK(&nh->lock);
	return ret;
}

/*
*
*/
INT mt_notify_chain_unregister(struct notify_head *nh, struct notify_entry *entry)
{
	INT ret;

	OS_SPIN_LOCK(&nh->lock);
	ret = raw_notify_chain_unregister(&nh->head, entry);
	OS_SPIN_UNLOCK(&nh->lock);
	return ret;
}

/*
*
*/
INT mt_notify_call_chain(struct notify_head *nh, INT val, VOID *v)
{
	INT ret;

	ret = raw_notify_call_chain(&nh->head, val, v);
	return ret;
}

