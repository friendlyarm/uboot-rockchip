/*
 * SPDX-License-Identifier:     GPL-2.0+
 *
 * Copyright (C) Guangzhou FriendlyELEC Computer Tech. Co., Ltd.
 * (http://www.friendlyarm.com)
 *
 * Copyright (c) 2020 Rockchip Electronics Co., Ltd
 */

#ifndef __CONFIG_NANOPI5_H__
#define __CONFIG_NANOPI5_H__

#include <configs/rk3568_common.h>

/* Remove or override few declarations from rk3568-common.h */
#undef CONFIG_BOOTCOMMAND
#undef CONFIG_DISPLAY_BOARDINFO_LATE
#undef RKIMG_DET_BOOTDEV
#undef RKIMG_BOOTCOMMAND

#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_SYS_MMC_MAX_BLK_COUNT	32768

#define CONFIG_MISC_INIT_R
#define CONFIG_SERIAL_TAG

#ifndef CONFIG_SPL_BUILD

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

#define RKIMG_BOOTCOMMAND \
	"boot_fit;" \
	"boot_android ${devtype} ${devnum};" \
	"bootrkp;" \
	"run distro_bootcmd;"

#define CONFIG_BOOTCOMMAND		RKIMG_BOOTCOMMAND

#endif

#endif /* __CONFIG_NANOPI5_H__ */
