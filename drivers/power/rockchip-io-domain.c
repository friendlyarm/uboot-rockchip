/*
 * (C) Copyright 2020 Fuzhou Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <errno.h>
#include <fdtdec.h>
#include <fdt_support.h>
#include <asm/arch/rkplat.h>
#include <io-domain.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_RKCHIP_RK3399
#define COMPAT_IO_DOMAIN	"rockchip,rk3399-io-voltage-domain"
#define COMPAT_IO_DOMAIN_PMU	"rockchip,rk3399-pmu-io-voltage-domain"
#define COMPAT_PROP_NAME	"uboot-set"
/*
 * Possible supplies for rk3399:
 * bit 0: bt656-supply:  The supply connected to APIO2_VDD
 * bit 1: audio-supply:  The supply connected to APIO5_VDD
 * bit 2: sdmmc-supply:  The supply connected to SDMMC0_VDD
 * bit 3: gpio1830    :  The supply connected to APIO4_VDD
 *
 * grf_writel(1 << 16, GRF_IO_VSEL);
 *
 * Possible supplies for rk3399 pmu-domains:
 * - pmu1830-supply:The supply connected to PMUIO2_VDD.
 *
 * bit 9: pmu1830_vol, pmu IO 1.8v/3.0v select.
 * 0: 3.0v
 * 1: 1.8v
 *
 * bit 8: pmu1830_volsel, pmu GPIO1 1.8v/3.0v control source select.
 * 0: controlled by IO_GPIO0B1 ;
 * 1: controlled by PMUGRF.SOC_CON0.pmu1830_vol
 *
 * pmugrf_writel(1 << 16, PMU_GRF_SOC_CON0);
*/
int rk3399_io_domain_init(void)
{
	const void *blob = gd->fdt_blob;
	int node = 0;
	int val = 0;

	if (!blob)
		return -1;

	node = fdt_node_offset_by_compatible(blob, 0, COMPAT_IO_DOMAIN);
	if (node) {
		val = fdtdec_get_int(blob, node, COMPAT_PROP_NAME, 0);
		if (val) {
			printf("%s: set %s %x\n", __func__, COMPAT_IO_DOMAIN, val);
			grf_writel(val, GRF_IO_VSEL);
		}
	}

	node = fdt_node_offset_by_compatible(blob, 0, COMPAT_IO_DOMAIN_PMU);
	if (node) {
		val = fdtdec_get_int(blob, node, COMPAT_PROP_NAME, 0);
		if (val) {
			printf("%s: set %s %x\n", __func__, COMPAT_IO_DOMAIN_PMU, val);
			pmugrf_writel(val, PMU_GRF_SOC_CON0);
		}
	}
	return 0;
}
#endif

int board_io_domain_init(void)
{
#ifdef CONFIG_RKCHIP_RK3399
	rk3399_io_domain_init();
#endif
}
