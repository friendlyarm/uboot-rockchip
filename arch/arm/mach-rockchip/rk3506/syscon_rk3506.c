/*
 * (C) Copyright 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <syscon.h>
#include <asm/arch/clock.h>

static const struct udevice_id rk3506_syscon_ids[] = {
	{ .compatible = "rockchip,rk3506-grf", .data = ROCKCHIP_SYSCON_GRF },
	{ .compatible = "rockchip,rk3506-ioc-grf", .data = ROCKCHIP_SYSCON_IOC },
	{ .compatible = "rockchip,rk3506-ioc1", .data = ROCKCHIP_SYSCON_IOC },
	{ .compatible = "rockchip,rk3506-grf-pmu", .data = ROCKCHIP_SYSCON_PMUGRF },
	{ .compatible = "rockchip,rk3506-ioc-pmu", .data = ROCKCHIP_SYSCON_IOC },
	{ }
};

U_BOOT_DRIVER(syscon_rk3506) = {
	.name = "rk3506_syscon",
	.id = UCLASS_SYSCON,
	.of_match = rk3506_syscon_ids,
#if !CONFIG_IS_ENABLED(OF_PLATDATA)
	.bind = dm_scan_fdt_dev,
#endif
};
