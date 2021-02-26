// SPDX-License-Identifier:     GPL-2.0
/*
 * Copyright (C) Guangzhou FriendlyARM Computer Tech. Co., Ltd.
 * (http://www.friendlyarm.com)
 *
 * Copyright (C) 2020 Rockchip Electronics Co., Ltd
 */

#include <common.h>
#include <dm.h>
#include <misc.h>
#include <asm/io.h>
#include <asm/setup.h>
#include <usb.h>
#include <dwc3-uboot.h>
#include <linux/usb/phy-rockchip-inno-usb3.h>
#include <u-boot/sha256.h>
#include <i2c.h>

#include "hwrev.h"

DECLARE_GLOBAL_DATA_PTR;

#define RK3328_CPUID_OFF  0x7
#define RK3328_CPUID_LEN  0x10

static int __maybe_unused mac_read_from_generic_eeprom(u8 *addr)
{
	struct udevice *i2c_dev;
	int ret;

	/* Microchip 24AA02xxx EEPROMs with EUI-48 Node Identity */
	ret = i2c_get_chip_for_busnum(0, 0x51, 1, &i2c_dev);
	if (!ret)
		ret = dm_i2c_read(i2c_dev, 0xfa, addr, 6);

	return ret;
}

static void setup_macaddr(void)
{
#if CONFIG_IS_ENABLED(CMD_NET)
	int ret;
	const char *cpuid = env_get("cpuid#");
	u8 hash[SHA256_SUM_LEN];
	int size = sizeof(hash);
	u8 mac_addr[6];
	int from_eeprom = 0;
	int lockdown = 0;

#ifndef CONFIG_ENV_IS_NOWHERE
	lockdown = env_get_yesno("lockdown") == 1;
#endif
	if (lockdown && env_get("ethaddr"))
		return;

	ret = mac_read_from_generic_eeprom(mac_addr);
	if (!ret && is_valid_ethaddr(mac_addr)) {
		eth_env_set_enetaddr("ethaddr", mac_addr);
		from_eeprom = 1;
	}

	if (!cpuid) {
		debug("%s: could not retrieve 'cpuid#'\n", __func__);
		return;
	}

	ret = hash_block("sha256", (void *)cpuid, strlen(cpuid), hash, &size);
	if (ret) {
		debug("%s: failed to calculate SHA256\n", __func__);
		return;
	}

	/* Copy 6 bytes of the hash to base the MAC address on */
	memcpy(mac_addr, hash, 6);

	/* Make this a valid MAC address and set it */
	mac_addr[0] &= 0xfe;  /* clear multicast bit */
	mac_addr[0] |= 0x02;  /* set local assignment bit (IEEE802) */

	if (from_eeprom) {
		eth_env_set_enetaddr("eth1addr", mac_addr);
	} else {
		eth_env_set_enetaddr("ethaddr", mac_addr);

		if (lockdown && env_get("eth1addr"))
			return;

		/* Ugly, copy another 4 bytes to generate a similar address */
		memcpy(mac_addr + 2, hash + 8, 4);
		if (!memcmp(hash + 2, hash + 8, 4))
			mac_addr[5] ^= 0xff;

		eth_env_set_enetaddr("eth1addr", mac_addr);
	}
#endif

	return;
}

static void setup_serial(void)
{
#if CONFIG_IS_ENABLED(ROCKCHIP_EFUSE)
	struct udevice *dev;
	int ret, i;
	u8 cpuid[RK3328_CPUID_LEN];
	u8 low[RK3328_CPUID_LEN/2], high[RK3328_CPUID_LEN/2];
	char cpuid_str[RK3328_CPUID_LEN * 2 + 1];
	u64 serialno;
	char serialno_str[16];

#ifndef CONFIG_ENV_IS_NOWHERE
	if (env_get_yesno("lockdown") == 1 &&
		env_get("cpuid#") && env_get("serial#"))
		return;
#endif

	/* retrieve the device */
	ret = uclass_get_device_by_driver(UCLASS_MISC,
					  DM_GET_DRIVER(rockchip_efuse), &dev);
	if (ret) {
		debug("%s: could not find efuse device\n", __func__);
		return;
	}

	/* read the cpu_id range from the efuses */
	ret = misc_read(dev, RK3328_CPUID_OFF, &cpuid, sizeof(cpuid));
	if (ret) {
		debug("%s: reading cpuid from the efuses failed\n",
		      __func__);
		return;
	}

	memset(cpuid_str, 0, sizeof(cpuid_str));
	for (i = 0; i < 16; i++)
		sprintf(&cpuid_str[i * 2], "%02x", cpuid[i]);

	debug("cpuid: %s\n", cpuid_str);

	/*
	 * Mix the cpuid bytes using the same rules as in
	 *   ${linux}/drivers/soc/rockchip/rockchip-cpuinfo.c
	 */
	for (i = 0; i < 8; i++) {
		low[i] = cpuid[1 + (i << 1)];
		high[i] = cpuid[i << 1];
	}

	serialno = crc32_no_comp(0, low, 8);
	serialno |= (u64)crc32_no_comp(serialno, high, 8) << 32;
	snprintf(serialno_str, sizeof(serialno_str), "%llx", serialno);

	env_set("cpuid#", cpuid_str);
	env_set("serial#", serialno_str);
#endif

	return;
}

int misc_init_r(void)
{
	setup_serial();
	setup_macaddr();

	return 0;
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

#if CONFIG_IS_ENABLED(USB_DWC3_GADGET)
static struct dwc3_device dwc3_device_data = {
	.maximum_speed = USB_SPEED_SUPER,
	.base = 0xff600000,
	.dr_mode = USB_DR_MODE_PERIPHERAL,
	.index = 0,
	.dis_u2_susphy_quirk = 1,
	.dis_u1u2_quirk = 1,
	.usb2_phyif_utmi_width = 16,
};

int usb_gadget_handle_interrupts(void)
{
	dwc3_uboot_handle_interrupt(0);
	return 0;
}

int board_usb_init(int index, enum usb_init_type init)
{
	rockchip_u3phy_uboot_init();
	return dwc3_uboot_init(&dwc3_device_data);
}
#endif

int board_select_fdt_index(ulong dt_table_hdr, struct blk_desc *dev_desc)
{
	return (dev_desc ? dev_desc->devnum : 0);
}

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
			"rk3328-nanopi-r2-rev%02x.dtb", get_board_rev());
	env_set("dtb_name", info);
}

#ifdef CONFIG_BOARD_LATE_INIT
int rk_board_late_init(void)
{
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

