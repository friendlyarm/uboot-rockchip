/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2023 Rockchip Electronics Co. Ltd.
 * Author:
 *	Finley Xiao <finley.xiao@rock-chips.com>
 */

#ifndef _ASM_ARCH_CRU_RK3506_H
#define _ASM_ARCH_CRU_RK3506_H

#define MHz		1000000
#define KHz		1000
#define OSC_HZ		(24 * MHz)

#define CPU_FREQ_HZ	589824000

/* RK3506 pll id */
enum rk3506_pll_id {
	GPLL,
	V0PLL,
	V1PLL,
	PLL_COUNT,
};

struct rk3506_clk_info {
	unsigned long id;
	char *name;
};

struct rk3506_clk_priv {
	struct rk3506_cru *cru;
	ulong gpll_hz;
	ulong gpll_div_hz;
	ulong gpll_div_100mhz;
	ulong v0pll_hz;
	ulong v0pll_div_hz;
	ulong v1pll_hz;
	ulong v1pll_div_hz;
	ulong armclk_hz;
	ulong armclk_enter_hz;
	ulong armclk_init_hz;
	bool sync_kernel;
	bool set_armclk_rate;
};

struct rk3506_cru {
	/* cru */
	uint32_t reserved0000[160];   /* offset 0x0   */
	uint32_t mode_con;            /* offset 0x280 */
	uint32_t reserved0284[31];    /* offset 0x284 */
	uint32_t clksel_con[62];      /* offset 0x300 */
	uint32_t reserved03f8[258];   /* offset 0x3F8 */
	uint32_t gate_con[23];        /* offset 0x800 */
	uint32_t reserved085c[105];   /* offset 0x85C */
	uint32_t softrst_con[23];     /* offset 0xA00 */
	uint32_t reserved0a5c[105];   /* offset 0xA5C */
	uint32_t glb_cnt_th;          /* offset 0xC00 */
	uint32_t glb_rst_st;          /* offset 0xC04 */
	uint32_t glb_srst_fst;        /* offset 0xC08 */
	uint32_t glb_srst_snd;        /* offset 0xC0C */
	uint32_t glb_rst_con;         /* offset 0xC10 */
	uint32_t reserved0c14[6];     /* offset 0xC14 */
	uint32_t corewfi_con;         /* offset 0xC2C */
	uint32_t reserved0c30[15604]; /* offset 0xC30 */

	/* pmu cru */
	uint32_t gpll_con[5];         /* offset 0x10000 */
	uint32_t reserved10014[3];    /* offset 0x10014 */
	uint32_t v0pll_con[5];        /* offset 0x10020 */
	uint32_t reserved10034[3];    /* offset 0x10034 */
	uint32_t v1pll_con[5];        /* offset 0x10040 */
	uint32_t reserved10074[171];  /* offset 0x10054 */
	uint32_t pmuclksel_con[7];    /* offset 0x10300 */
	uint32_t reserved1031c[313];  /* offset 0x1031C */
	uint32_t pmugate_con[3];      /* offset 0x10800 */
	uint32_t reserved1080c[125];  /* offset 0x1080C */
	uint32_t pmusoftrst_con[2];   /* offset 0x10A00 */
	uint32_t reserved10a08[7583]; /* offset 0x10A08 */
};

check_member(rk3506_cru, reserved0c30[0], 0x0c30);
check_member(rk3506_cru, reserved10a08[0], 0x10a08);

struct pll_rate_table {
	unsigned long rate;
	unsigned int fbdiv;
	unsigned int postdiv1;
	unsigned int refdiv;
	unsigned int postdiv2;
	unsigned int dsmpd;
	unsigned int frac;
};

#define RK3506_PMU_CRU_BASE		0x10000
#define RK3506_PLL_CON(x)		((x) * 0x4 + RK3506_PMU_CRU_BASE)
#define RK3506_CLKSEL_CON(x)		((x) * 0x4 + 0x300)
#define RK3506_CLKGATE_CON(x)		((x) * 0x4 + 0x800)
#define RK3506_SOFTRST_CON(x)		((x) * 0x4 + 0xa00)
#define RK3506_PMU_CLKSEL_CON(x)	((x) * 0x4 + 0x300 + RK3506_PMU_CRU_BASE)
#define RK3506_PMU_CLKGATE_CON(x)	((x) * 0x4 + 0x800 + RK3506_PMU_CRU_BASE)
#define RK3506_MODE_CON			0x280
#define RK3506_GLB_CNT_TH		0xc00
#define RK3506_GLB_SRST_FST		0xc08
#define RK3506_GLB_SRST_SND		0xc0c

enum {
	/* CRU_CLKSEL_CON00 */
	CLK_GPLL_DIV_SHIFT		= 6,
	CLK_GPLL_DIV_MASK		= 0xf << CLK_GPLL_DIV_SHIFT,
	CLK_GPLL_DIV_100M_SHIFT		= 10,
	CLK_GPLL_DIV_100M_MASK		= 0xf << CLK_GPLL_DIV_100M_SHIFT,

	/* CRU_CLKSEL_CON01 */
	CLK_V0PLL_DIV_SHIFT		= 0,
	CLK_V0PLL_DIV_MASK		= 0xf << CLK_V0PLL_DIV_SHIFT,
	CLK_V1PLL_DIV_SHIFT		= 4,
	CLK_V1PLL_DIV_MASK		= 0xf << CLK_V1PLL_DIV_SHIFT,

	/* CRU_CLKSEL_CON15 */
	CLK_CORE_SRC_DIV_SHIFT		= 0,
	CLK_CORE_SRC_DIV_MASK		= 0x1f << CLK_CORE_SRC_DIV_SHIFT,
	CLK_CORE_SRC_SEL_SHIFT		= 5,
	CLK_CORE_SRC_SEL_MASK		= 0x3 << CLK_CORE_SRC_SEL_SHIFT,
	CLK_CORE_SEL_GPLL		= 0,
	CLK_CORE_SEL_V0PLL,
	CLK_CORE_SEL_V1PLL,
	CLK_CORE_SRC_PVTMUX_SEL_SHIFT	= 8,
	CLK_CORE_SRC_PVTMUX_SEL_MASK	= 0x1 << CLK_CORE_SRC_PVTMUX_SEL_SHIFT,
	CLK_CORE_SRC_PRE		= 0,
	CLK_CORE_PVTPLL_SRC,

	ACLK_CORE_DIV_SHIFT		= 9,
	ACLK_CORE_DIV_MASK		= 0xf << ACLK_CORE_DIV_SHIFT,

	/* CRU_CLKSEL_CON16 */
	PCLK_CORE_DIV_SHIFT		= 0,
	PCLK_CORE_DIV_MASK		= 0xf << PCLK_CORE_DIV_SHIFT,

	/* CRU_CLKSEL_CON21 */
	ACLK_BUS_DIV_SHIFT		= 0,
	ACLK_BUS_DIV_MASK		= 0x1f << ACLK_BUS_DIV_SHIFT,
	ACLK_BUS_SEL_SHIFT		= 5,
	ACLK_BUS_SEL_MASK		= 0x3 << ACLK_BUS_SEL_SHIFT,
	ACLK_BUS_SEL_GPLL_DIV		= 0,
	ACLK_BUS_SEL_V0PLL_DIV,
	ACLK_BUS_SEL_V1PLL_DIV,

	HCLK_BUS_DIV_SHIFT		= 7,
	HCLK_BUS_DIV_MASK		= 0x1f << HCLK_BUS_DIV_SHIFT,
	HCLK_BUS_SEL_SHIFT		= 12,
	HCLK_BUS_SEL_MASK		= 0x3 << HCLK_BUS_SEL_SHIFT,

	/* CRU_CLKSEL_CON22 */
	PCLK_BUS_DIV_SHIFT		= 0,
	PCLK_BUS_DIV_MASK		= 0x1f << PCLK_BUS_DIV_SHIFT,
	PCLK_BUS_SEL_SHIFT		= 5,
	PCLK_BUS_SEL_MASK		= 0x3 << PCLK_BUS_SEL_SHIFT,

	/* CRU_CLKSEL_CON29 */
	HCLK_LSPERI_DIV_SHIFT		= 0,
	HCLK_LSPERI_DIV_MASK		= 0x1f << HCLK_LSPERI_DIV_SHIFT,
	HCLK_LSPERI_SEL_SHIFT		= 5,
	HCLK_LSPERI_SEL_MASK		= 0x3 << HCLK_LSPERI_SEL_SHIFT,

	/* CRU_CLKSEL_CON32 */
	CLK_I2C0_DIV_SHIFT		= 0,
	CLK_I2C0_DIV_MASK		= 0xf << CLK_I2C0_DIV_SHIFT,
	CLK_I2C0_SEL_SHIFT		= 4,
	CLK_I2C0_SEL_MASK		= 0x3 << CLK_I2C0_SEL_SHIFT,
	CLK_I2C_SEL_GPLL		= 0,
	CLK_I2C_SEL_V0PLL,
	CLK_I2C_SEL_V1PLL,
	CLK_I2C1_DIV_SHIFT		= 6,
	CLK_I2C1_DIV_MASK		= 0xf << CLK_I2C1_DIV_SHIFT,
	CLK_I2C1_SEL_SHIFT		= 10,
	CLK_I2C1_SEL_MASK		= 0x3 << CLK_I2C1_SEL_SHIFT,

	/* CRU_CLKSEL_CON33 */
	CLK_I2C2_DIV_SHIFT		= 0,
	CLK_I2C2_DIV_MASK		= 0xf << CLK_I2C2_DIV_SHIFT,
	CLK_I2C2_SEL_SHIFT		= 4,
	CLK_I2C2_SEL_MASK		= 0x3 << CLK_I2C2_SEL_SHIFT,
	CLK_PWM1_DIV_SHIFT		= 6,
	CLK_PWM1_DIV_MASK		= 0xf << CLK_PWM1_DIV_SHIFT,
	CLK_PWM1_SEL_SHIFT		= 10,
	CLK_PWM1_SEL_MASK		= 0x3 << CLK_PWM1_SEL_SHIFT,
	CLK_PWM1_SEL_GPLL_DIV		= 0,
	CLK_PWM1_SEL_V0PLL_DIV,
	CLK_PWM1_SEL_V1PLL_DIV,

	/* CRU_CLKSEL_CON34 */
	CLK_SPI0_DIV_SHIFT		= 4,
	CLK_SPI0_DIV_MASK		= 0xf << CLK_SPI0_DIV_SHIFT,
	CLK_SPI0_SEL_SHIFT		= 8,
	CLK_SPI0_SEL_MASK		= 0x3 << CLK_SPI0_SEL_SHIFT,
	CLK_SPI_SEL_24M			= 0,
	CLK_SPI_SEL_GPLL_DIV,
	CLK_SPI_SEL_V0PLL_DIV,
	CLK_SPI_SEL_V1PLL_DIV,

	CLK_SPI1_DIV_SHIFT		= 10,
	CLK_SPI1_DIV_MASK		= 0xf << CLK_SPI1_DIV_SHIFT,
	CLK_SPI1_SEL_SHIFT		= 14,
	CLK_SPI1_SEL_MASK		= 0x3 << CLK_SPI1_SEL_SHIFT,

	/* CRU_CLKSEL_CON49 */
	ACLK_HSPERI_DIV_SHIFT		= 0,
	ACLK_HSPERI_DIV_MASK		= 0x1f << ACLK_HSPERI_DIV_SHIFT,
	ACLK_HSPERI_SEL_SHIFT		= 5,
	ACLK_HSPERI_SEL_MASK		= 0x3 << ACLK_HSPERI_SEL_SHIFT,
	ACLK_HSPERI_SEL_GPLL_DIV	= 0,
	ACLK_HSPERI_SEL_V0PLL_DIV	= 1,
	ACLK_HSPERI_SEL_V1PLL_DIV	= 2,

	CCLK_SDMMC_DIV_SHIFT		= 7,
	CCLK_SDMMC_DIV_MASK		= 0x3f << CCLK_SDMMC_DIV_SHIFT,
	CCLK_SDMMC_SEL_SHIFT		= 13,
	CCLK_SDMMC_SEL_MASK		= 0x3 << CCLK_SDMMC_SEL_SHIFT,
	CCLK_SDMMC_SEL_24M		= 0,
	CCLK_SDMMC_SEL_GPLL,
	CCLK_SDMMC_SEL_V0PLL,
	CCLK_SDMMC_SEL_V1PLL,

	/* CRU_CLKSEL_CON50 */
	SCLK_FSPI_DIV_SHIFT		= 0,
	SCLK_FSPI_DIV_MASK		= 0x1f << SCLK_FSPI_DIV_SHIFT,
	SCLK_FSPI_SEL_SHIFT		= 5,
	SCLK_FSPI_SEL_MASK		= 0x3 << SCLK_FSPI_SEL_SHIFT,
	SCLK_FSPI_SEL_24M		= 0,
	SCLK_FSPI_SEL_GPLL,
	SCLK_FSPI_SEL_V0PLL,
	SCLK_FSPI_SEL_V1PLL,
	CLK_MAC_DIV_SHIFT		= 7,
	CLK_MAC_DIV_MASK		= 0x1f << CLK_MAC_DIV_SHIFT,

	/* CRU_CLKSEL_CON54 */
	CLK_SARADC_DIV_SHIFT		= 0,
	CLK_SARADC_DIV_MASK		= 0xf << CLK_SARADC_DIV_SHIFT,
	CLK_SARADC_SEL_SHIFT		= 4,
	CLK_SARADC_SEL_MASK		= 0x3 << CLK_SARADC_SEL_SHIFT,
	CLK_SARADC_SEL_24M		= 0,
	CLK_SARADC_SEL_400K,
	CLK_SARADC_SEL_32K,

	/* CRU_CLKSEL_CON60 */
	DCLK_VOP_DIV_SHIFT		= 0,
	DCLK_VOP_DIV_MASK		= 0xff << DCLK_VOP_DIV_SHIFT,
	DCLK_VOP_SEL_SHIFT		= 8,
	DCLK_VOP_SEL_MASK		= 0x7 << DCLK_VOP_SEL_SHIFT,
	DCLK_VOP_SEL_24M		= 0,
	DCLK_VOP_SEL_GPLL,
	DCLK_VOP_SEL_V0PLL,
	DCLK_VOP_SEL_V1PLL,
	DCLK_VOP_SEL_FRAC_VOIC1,
	DCLK_VOP_SEL_FRAC_COMMON0,
	DCLK_VOP_SEL_FRAC_COMMON1,
	DCLK_VOP_SEL_FRAC_COMMON2,

	/* CRU_CLKSEL_CON61 */
	CLK_TSADC_DIV_SHIFT		= 0,
	CLK_TSADC_DIV_MASK		= 0xff << CLK_TSADC_DIV_SHIFT,
	CLK_TSADC_TSEN_DIV_SHIFT	= 8,
	CLK_TSADC_TSEN_DIV_MASK		= 0x7 << CLK_TSADC_TSEN_DIV_SHIFT,

	/* PMUCRU_CLKSEL_CON00 */
	CLK_PWM0_DIV_SHIFT		= 6,
	CLK_PWM0_DIV_MASK		= 0xf << CLK_PWM0_DIV_SHIFT,
	CLK_MAC_OUT_DIV_SHIFT		= 10,
	CLK_MAC_OUT_DIV_MASK		= 0x3f << CLK_MAC_OUT_DIV_SHIFT,

};
#endif
