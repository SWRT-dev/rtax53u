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
#ifndef _MACH_BCM963XX_BARRIERS_H
#define _MACH_BCM963XX_BARRIERS_H

#include <asm/outercache.h>

#ifdef CONFIG_BCM_B15_MEGA_BARRIER
void BcmMegaBarrier(void); /* Implemented in board_963xx.c */
#endif

#if defined(CONFIG_BCM_KF_SPECTRE_PATCH) && defined(CONFIG_BCM_SPECTRE_PATCH_ENABLE)
extern void arm_heavy_mb(void);
#endif

#define isb() __asm__ __volatile__ ("isb" : : : "memory")
#define dsb() __asm__ __volatile__ ("dsb" : : : "memory")
#define dmb() __asm__ __volatile__ ("dmb" : : : "memory")

#if defined(CONFIG_ARM_DMA_MEM_BUFFERABLE) || defined(CONFIG_SMP)
 #if defined(CONFIG_BCM_B15_MEGA_BARRIER)
  #if defined(CONFIG_BCM_KF_SPECTRE_PATCH) && defined(CONFIG_BCM_SPECTRE_PATCH_ENABLE)
   #define mb()		do { BcmMegaBarrier(); arm_heavy_mb(); } while (0)
   #define wmb()	do { BcmMegaBarrier(); arm_heavy_mb(); } while (0)
  #else
   #define mb()		BcmMegaBarrier()
   #define wmb()	BcmMegaBarrier()
  #endif
 #else
  #if defined(CONFIG_BCM_KF_SPECTRE_PATCH) && defined(CONFIG_BCM_SPECTRE_PATCH_ENABLE)
   #define mb()		do { dsb(); arm_heavy_mb(); } while (0)
  #else
   #define mb()		do { dsb(); outer_sync(); } while (0)
  #endif
  #define wmb()		mb()
 #endif
 #define rmb()		dsb()
#else
 #include <asm/memory.h>
 #define mb()	do { if (arch_is_coherent()) dmb(); else barrier(); } while (0)
 #define rmb()	do { if (arch_is_coherent()) dmb(); else barrier(); } while (0)
 #define wmb()	do { if (arch_is_coherent()) dmb(); else barrier(); } while (0)
#endif

#ifndef CONFIG_SMP
 #ifdef CONFIG_BCM_B15_MEGA_BARRIER
  #define smp_mb()	BcmMegaBarrier()
 #else
  #define smp_mb()	barrier()
 #endif
 #define smp_rmb()	barrier()
 #define smp_wmb()	smp_mb()
#else /*CONFIG_SMP:*/
 #if defined(CONFIG_BCM_B15_MEGA_BARRIER)
  #define smp_mb()	BcmMegaBarrier()
  #define smp_rmb()	dmb(ish)
  #define smp_wmb()	BcmMegaBarrier()
 #else
  #define smp_mb()	dmb()
  #define smp_rmb()	dmb()
  #define smp_wmb()	dmb()
 #endif
#endif /*CONFIG_SMP*/
#endif /*_MACH_BCM963XX_BARRIERS_H*/
#endif /*(CONFIG_BCM_KF_ARM_BCM963XX)*/
