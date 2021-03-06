#ifndef __MT_CHADR_H__
#define __MT_CHADR_H__

#ifdef  MT_GLOBAL
#define MT_EXTERN
#define MT_I(x) x
#else
#define MT_EXTERN extern
#define MT_I(x)
#endif

/* unit base addresses */
#define MT_LOCAL_MIPS_BASE_ADDRESS           0xb4900000 //0xa7000000 //HUANX
#define REG_CPU_SUB_CLK_GATE		0x0
#define CPU_SUB_CLK_GATE_MASK_ALL	0x3F
#define REG_CPU_SUB_RST			0x4
#define CPU_SUB_RST_MASK_RST		0x3F

/* for UART mux control for different platform*/
#ifdef CONFIG_SOC_GRX500_BOOTCORE
#define LTQ_CHIPTOP_BASE_ADDR		0x16080000
#define LTQ_CHIPTOP			KSEG1ADDR(LTQ_CHIPTOP_BASE_ADDR)
#define LTQ_IFMUX_CFG			((u32 *)(LTQ_CHIPTOP + 0x0120))
//! Field BBTEPUART_EN - Enable Baseband TEP ASC1 UART
#define IFMUX_CFG_BBTEPUART_EN_POS 10
//! Field BBTEPUART_EN - Enable Baseband TEP ASC1 UART
#define IFMUX_CFG_BBTEPUART_EN_MASK 0xC00u
//! Constant ASC1_U - ASC1
#define CONST_IFMUX_CFG_BBTEPUART_EN_ASC1_U 0x0
//! Constant WLAN_U - Enabled
#define CONST_IFMUX_CFG_BBTEPUART_EN_WLAN_U 0x1
//! Constant TEP_U - UART inside TEP sub system
#define CONST_IFMUX_CFG_BBTEPUART_EN_TEP_U 0x2
#define LTQ_MUX_TEP			(CONST_IFMUX_CFG_BBTEPUART_EN_TEP_U<<IFMUX_CFG_BBTEPUART_EN_POS)
#endif /* CONFIG_SOC_GRX500_BOOTCORE */

#ifdef CONFIG_SOC_PRX300_BOOTCORE
#define LTQ_GPIO_PAD_CTRL_BASE_ADDR	0x16310000
#define LTQ_GPIO_PAD_CTRL		KSEG1ADDR(LTQ_GPIO_PAD_CTRL_BASE_ADDR)
#define LTQ_PORTMUXC30			((u32 *)(LTQ_GPIO_PAD_CTRL + 0x0078))
#define LTQ_PORTMUXC31			((u32 *)(LTQ_GPIO_PAD_CTRL + 0x007C))
#define LTQ_MUX_TEP			0x4
#endif /* CONFIG_SOC_PRX300_BOOTCORE  */


#undef MT_EXTERN
#undef MT_I

#endif /* __MT_CHADR_H__ */ 
