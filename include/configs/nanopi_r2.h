/*
 * Copyright (C) Guangzhou FriendlyELEC Computer Tech. Co., Ltd.
 * (http://www.friendlyarm.com)
 *
 * (C) Copyright 2016 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __CONFIG_NANOPI_R2_H__
#define __CONFIG_NANOPI_R2_H__

#include <configs/rk3328_common.h>

/* Remove or override few declarations from rk3399-common.h */
#undef CONFIG_BOOTCOMMAND
#undef CONFIG_DISPLAY_BOARDINFO_LATE
#undef RKIMG_DET_BOOTDEV

/* SD/MMC */
#define CONFIG_SYS_MMC_ENV_DEV		1

#define SDRAM_BANK_SIZE			(2UL << 30)
#define CONFIG_MISC_INIT_R
#define CONFIG_SERIAL_TAG
#define CONFIG_ENV_OVERWRITE

#define CONFIG_BMP_16BPP
#define CONFIG_BMP_24BPP
#define CONFIG_BMP_32BPP

#ifndef CONFIG_SPL_BUILD

/* Monitor Command Prompt */
#undef CONFIG_SYS_PROMPT
#define CONFIG_SYS_PROMPT		"nanopi-r2# "

/*---------------------------------------------------------------
 * ENV settings
 */

#define ROCKCHIP_DEVICE_SETTINGS \
	"stdout=serial,vidconsole\0" \
	"stderr=serial,vidconsole\0"

#define RKIMG_DET_BOOTDEV \
	"rkimg_bootdev=" \
	"if mmc dev 1 && rkimgtest mmc 1; then " \
		"setenv devtype mmc; setenv devnum 1; echo Boot from SDcard;" \
	"elif mmc dev 0; then " \
		"setenv devtype mmc; setenv devnum 0;" \
	"elif rksfc dev 1; then " \
		"setenv devtype spinor; setenv devnum 1;" \
	"fi; \0"

#define CONFIG_BOOTCOMMAND \
	"bootrkp; run distro_bootcmd"

#endif /* CONFIG_SPL_BUILD */

#endif /* __CONFIG_NANOPI_R2_H__ */
