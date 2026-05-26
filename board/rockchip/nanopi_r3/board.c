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
#include <usb.h>
#include <dwc3-uboot.h>

#include <dm/device.h>
#include <dm/ofnode.h>
#include <dm/read.h>
#include <fdt_support.h>

#include "hwrev.h"

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_USB_DWC3
static struct dwc3_device dwc3_device_data = {
	.maximum_speed = USB_SPEED_HIGH,
	.base = 0xfcc00000,
	.dr_mode = USB_DR_MODE_PERIPHERAL,
	.index = 0,
	.dis_u2_susphy_quirk = 1,
	.usb2_phyif_utmi_width = 16,
};

int usb_gadget_handle_interrupts(void)
{
	dwc3_uboot_handle_interrupt(0);
	return 0;
}

int board_usb_init(int index, enum usb_init_type init)
{
	return dwc3_uboot_init(&dwc3_device_data);
}
#endif

/* Supported panels and dpi for nanopi_r3 series */
static char *panels[] = {
	"HD702E,213dpi",
	"HD703E,213dpi",
	"HDMI1024x768,165dpi",
	"HDMI1280x800,168dpi",
};

char *board_get_panel_name(void)
{
	char *name;
	int i;

	name = env_get("panel");
	if (!name)
		return NULL;

	for (i = 0; i < ARRAY_SIZE(panels); i++) {
		if (!strncmp(panels[i], name, strlen(name)))
			return panels[i];
	}

	return name;
}

int board_select_fdt_index(ulong dt_table_hdr, struct blk_desc *dev_desc)
{
	return (dev_desc ? dev_desc->devnum : 0);
}

bool board_use_kernel_dtb(const void *fdt)
{
	const char *prop;
	int offset, len;

	offset = fdt_path_offset(fdt, "/board");
	if (offset) {
		prop = fdt_getprop(fdt, offset, "uboot,skip-init-kdtb", &len);
		if (prop) {
			printf("kdtb: loaded\n");
			return false;
		}
	}

	return true;
}

static int board_check_supply(void)
{
	u32 adc_reading = 0;
	int mv = 5000;

	adc_channel_single_shot("saradc", 2, &adc_reading);
	debug("ADC reading %d\n", adc_reading);

	/* should be in 3v ~ 20v */
	if (adc_reading > 160 && adc_reading < 1024) {
		mv = adc_reading * 196 - 2130;
		mv /= 10;
	}

	printf("vdd_usbc %d mV\n", mv);

	return 0;
}

static int mac_read_from_generic_eeprom(u8 *addr)
{
	struct udevice *i2c_dev;
	int ret;

	/* Microchip 24AA02xxx EEPROMs with EUI-48 Node Identity */
	ret = i2c_get_chip_for_busnum(1, 0x53, 1, &i2c_dev);
	if (!ret)
		ret = dm_i2c_read(i2c_dev, 0xfa, addr, 6);

	return ret;
}

static void __maybe_unused setup_macaddr(void)
{
	u8 mac_addr[6] = { 0 };
	int lockdown = 0;
	int ret;

#ifndef CONFIG_ENV_IS_NOWHERE
	lockdown = env_get_yesno("lockdown") == 1;
#endif
	if (lockdown && env_get("ethaddr"))
		return;

	ret = mac_read_from_generic_eeprom(mac_addr);
	if (!ret && is_valid_ethaddr(mac_addr)) {
		debug("MAC: %pM\n", mac_addr);
		eth_env_set_enetaddr("ethaddr", mac_addr);
	}
}

#ifdef CONFIG_MISC_INIT_R
int misc_init_r(void)
{
	setup_macaddr();

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

	if (get_soc_id() == 0x3568) {
		snprintf(info, ARRAY_SIZE(info),
				"rk3568-nanopi5-rev%02x.dtb", get_board_rev());
	} else {
		snprintf(info, ARRAY_SIZE(info),
				"rk3566-nanopi-r3-rev%02x.dtb", get_board_rev());
	}
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

int rk_board_panel_detect(struct udevice *dev)
{
	struct udevice *i2c_bus, *nvm_dev;
	struct ofnode_phandle_args args;
	const struct device_node *np;
	const char *panel_name;
	u8 chip, addr, nlen;
	u8 buf[128] = { 0 };
	u32 status = 1;
	int ret;

	panel_name = dev_read_string(dev, "panel-name");
	if (!panel_name)
		return 0;

	ret = dev_read_phandle_with_args(dev, "nvmems", NULL, 3, 0, &args);
	if (ret)
		return 0;

	chip = args.args[0];
	addr = args.args[1];
	nlen = args.args[2];
	if (!chip || !addr)
		return 0;

	if (uclass_get_device_by_ofnode(UCLASS_I2C, args.node, &i2c_bus))
		return 0;

	if (i2c_get_chip(i2c_bus, chip, 1, &nvm_dev))
		return 0;

	if (nlen >= sizeof(buf))
		nlen = sizeof(buf) - 1;

	ret = dm_i2c_read(nvm_dev, addr, buf, nlen);
	if (!ret) {
		debug("panel: nvmem name %s\n", buf);
		nlen = strchrnul(panel_name, ',') - panel_name;
		if (!strncmp((char *)buf, panel_name, nlen))
			return 1;
	}

	np = ofnode_to_np(dev->node);
	if (!ofnode_read_u32(dev->node, "nvmem-status", &status)) {
		do_fixup_by_path_u32((void *)gd->fdt_blob, np->full_name,
				     "nvmem-status", 0, 1);
	}

	/* read fail or name mismatch */
	return -ENODEV;
}
