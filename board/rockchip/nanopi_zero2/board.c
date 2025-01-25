/*
 * SPDX-License-Identifier:     GPL-2.0+
 *
 * Copyright (c) 2024 FriendlyElec Computer Tech. Co., Ltd.
 * (http://www.friendlyelec.com)
 */

#include <common.h>
#include <adc.h>
#include <i2c.h>
#include <misc.h>
#include <asm/setup.h>
#include <dwc3-uboot.h>
#include <usb.h>
#include <linux/usb/phy-rockchip-naneng-combphy.h>
#include <asm/io.h>
#include <rockusb.h>

#include "hwrev.h"

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_USB_DWC3
#define CRU_BASE		0xff4a0000
#define CRU_SOFTRST_CON33	0x0a84
#define U3PHY_BASE		0xffdc0000

static struct dwc3_device dwc3_device_data = {
	.maximum_speed = USB_SPEED_SUPER,
	.base = 0xfe500000,
	.dr_mode = USB_DR_MODE_PERIPHERAL,
	.index = 0,
	.dis_u2_susphy_quirk = 1,
	.dis_u1u2_quirk = 1,
	.usb2_phyif_utmi_width = 16,
};

int usb_gadget_handle_interrupts(int index)
{
	dwc3_uboot_handle_interrupt(0);
	return 0;
}

bool rkusb_usb3_capable(void)
{
	return true;
}

static void usb_reset_otg_controller(void)
{
	writel(0x00020002, CRU_BASE + CRU_SOFTRST_CON33);
	mdelay(1);
	writel(0x00020000, CRU_BASE + CRU_SOFTRST_CON33);
	mdelay(1);
}

int board_usb_init(int index, enum usb_init_type init)
{
	u32 ret = 0;

	usb_reset_otg_controller();

#if defined(CONFIG_SUPPORT_USBPLUG)
	dwc3_device_data.maximum_speed = USB_SPEED_HIGH;

	if (rkusb_switch_usb3_enabled()) {
		dwc3_device_data.maximum_speed = USB_SPEED_SUPER;
		ret = rockchip_combphy_usb3_uboot_init(U3PHY_BASE);
		if (ret) {
			rkusb_force_to_usb2(true);
			dwc3_device_data.maximum_speed = USB_SPEED_HIGH;
		}
	}
#else
	ret = rockchip_combphy_usb3_uboot_init(U3PHY_BASE);
	if (ret) {
		rkusb_force_to_usb2(true);
		dwc3_device_data.maximum_speed = USB_SPEED_HIGH;
	}
#endif

	return dwc3_uboot_init(&dwc3_device_data);
}

#if defined(CONFIG_SUPPORT_USBPLUG)
int board_usb_cleanup(int index, enum usb_init_type init)
{
	dwc3_uboot_exit(index);
	return 0;
}
#endif

#endif

int board_select_fdt_index(ulong dt_table_hdr, struct blk_desc *dev_desc)
{
	return (dev_desc ? dev_desc->devnum : 0);
}

static int board_check_supply(void)
{
	u32 adc_reading = 0;
	adc_channel_single_shot("saradc", 3, &adc_reading);
	debug("ADC reading %d\n", adc_reading);

	return 0;
}

#ifdef CONFIG_MISC_INIT_R
int misc_init_r(void)
{
	return 0;
}
#endif

#ifdef CONFIG_DISPLAY_BOARDINFO
int show_board_info(void)
{
	printf("Board: %s\n", get_board_name());

	return 0;
}
#endif

#ifdef CONFIG_REVISION_TAG
static void set_board_rev(void)
{
	char info[64] = {0, };

	snprintf(info, ARRAY_SIZE(info), "%02x", get_board_rev());
	env_set("board_rev", info);
}
#endif

void set_dtb_name(void)
{
	char info[64] = {0, };

#ifndef CONFIG_ENV_IS_NOWHERE
	if (env_get_yesno("lockdown") == 1 &&
		env_get("dtb_name"))
		return;
#endif

	snprintf(info, ARRAY_SIZE(info),
			"rk3528-nanopi-rev%02x.dtb", get_board_rev());
	env_set("dtb_name", info);
}

#ifdef CONFIG_SERIAL_TAG
void get_board_serial(struct tag_serialnr *serialnr)
{
	char *serial_string;
	u64 serial = 0;

	serial_string = env_get("serial#");

	if (serial_string)
		serial = simple_strtoull(serial_string, NULL, 16);

	serialnr->high = (u32)(serial >> 32);
	serialnr->low = (u32)(serial & 0xffffffff);
}
#endif

#ifdef CONFIG_BOARD_LATE_INIT
int rk_board_late_init(void)
{
	board_check_supply();

#ifdef CONFIG_REVISION_TAG
	set_board_rev();
#endif

#ifdef CONFIG_SILENT_CONSOLE
	gd->flags &= ~GD_FLG_SILENT;
#endif

	printf("\n");

	return 0;
}
#endif
