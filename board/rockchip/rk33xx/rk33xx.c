/*
 * (C) Copyright 2008-2015 Fuzhou Rockchip Electronics Co., Ltd
 * Peter, Software Engineering, <superpeter.cai@gmail.com>.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <version.h>
#include <errno.h>
#include <fastboot.h>
#include <fdtdec.h>
#include <fdt_support.h>
#include <power/pmic.h>
#include <u-boot/sha256.h>
#include <hash.h>

#include <asm/io.h>
#include <asm/arch/rkplat.h>

#include "../common/config.h"
#ifdef CONFIG_OPTEE_CLIENT
#include "../common/rkloader/attestation_key.h"
#endif

#ifndef BIT
#define BIT(nr)			(1UL << (nr))
#endif

DECLARE_GLOBAL_DATA_PTR;

int __weak rkclk_set_apll_high(void)
{
	return 0;
}

static ulong get_sp(void)
{
	ulong ret;

	asm("mov %0, sp" : "=r"(ret) : );
	return ret;
}

void board_lmb_reserve(struct lmb *lmb) {
	ulong sp;
	sp = get_sp();
	debug("## Current stack ends at 0x%08lx ", sp);

	/* adjust sp by 64K to be safe */
	sp -= 64<<10;
	lmb_reserve(lmb, sp,
			gd->bd->bi_dram[0].start + gd->bd->bi_dram[0].size - sp);

	//reserve 48M for kernel & 8M for nand api.
	lmb_reserve(lmb, gd->bd->bi_dram[0].start, CONFIG_LMB_RESERVE_SIZE);
}

int board_storage_init(void)
{
	if (StorageInit() == 0) {
		puts("storage init OK!\n");
		return 0;
	} else {
		puts("storage init fail!\n");
		return -1;
	}
}


/*
 * ID info:
 *  ID : Volts : ADC value :   Bucket
 *  ==   =====   =========   ===========
 *   0 : 0.102V:        58 :    0 -   81
 *
 *  ------------------------------------
 *  Reserved
 *   1 : 0.211V:       120 :   82 -  150
 *   2 : 0.319V:       181 :  151 -  211
 *   3 : 0.427V:       242 :  212 -  274
 *   4 : 0.542V:       307 :  275 -  342
 *   5 : 0.666V:       378 :  343 -  411
 *   6 : 0.781V:       444 :  412 -  477
 *   7 : 0.900V:       511 :  478 -  545
 *   8 : 1.023V:       581 :  546 -  613
 *   9 : 1.137V:       646 :  614 -  675
 *  10 : 1.240V:       704 :  676 -  733
 *  11 : 1.343V:       763 :  734 -  795
 *  12 : 1.457V:       828 :  796 -  861
 *  13 : 1.576V:       895 :  862 -  925
 *  14 : 1.684V:       956 :  926 -  989
 *  15 : 1.800V:      1023 :  990 - 1023
 */
static const int id_readings[] = {
	 81, 150, 211, 274, 342, 411, 477, 545,
	613, 675, 733, 795, 861, 925, 989, 1023
};

static int cached_board_id = -1;

#define SARADC_BASE		0xFF100000
#define SARADC_DATA		(SARADC_BASE + 0)
#define SARADC_CTRL		(SARADC_BASE + 8)

static u32 get_saradc_value(int chn)
{
	int timeout = 0;
	u32 adc_value;

	writel(0, SARADC_CTRL);
	udelay(2);

	writel(0x28 | chn, SARADC_CTRL);
	udelay(50);

	timeout = 0;
	do {
		if (readl(SARADC_CTRL) & 0x40) {
			adc_value = readl(SARADC_DATA) & 0x3FF;
			return adc_value;
		}

		udelay(10);
	} while (timeout++ < 100);

	return -1;
}

static uint32_t get_adc_index(int chn)
{
	int i;
	int adc_reading;

	if (cached_board_id != -1)
		return cached_board_id;

	adc_reading = get_saradc_value(chn);
	for (i = 0; i < ARRAY_SIZE(id_readings); i++) {
		if (adc_reading <= id_readings[i]) {
			debug("ADC reading %d, ID %d\n", adc_reading, i);
			cached_board_id = i;
			return i;
		}
	}

	/* should die for impossible value */
	return 0;
}


/*
 * Board revision list: <GPIO4_D1 | GPIO4_D0>
 *  0b00 - NanoPC-T4
 *  0b01 - NanoPi M4
 *
 *  0b03 - Extended by ADC_IN4
 *  0b04 - NanoPi NEO4
 */
static int pcb_rev = -1;

static void bd_hwrev_init(void)
{
	gpio_direction_input(GPIO_BANK4 | GPIO_D0);
	gpio_direction_input(GPIO_BANK4 | GPIO_D1);

	pcb_rev  =  gpio_get_value(GPIO_BANK4 | GPIO_D0);
	pcb_rev |= (gpio_get_value(GPIO_BANK4 | GPIO_D1) << 1);

	if (pcb_rev == 0x3)
		pcb_rev += get_adc_index(4) + 1;
}

/* To override __weak symbols */
u32 get_board_rev(void)
{
	return pcb_rev;
}

static void set_dtb_name(void)
{
	char info[64] = {0, };

	snprintf(info, ARRAY_SIZE(info),
			"rk3399-nanopi4-rev%02x.dtb", get_board_rev());
	setenv("dtb_name", info);
}

/* PWM0/GPIO4_C2 */
static int panel_pwm_status = 0;

static void panel_pwm_status_init(void)
{
#define GPIO_PWM0	(GPIO_BANK4 | GPIO_C2)

	gpio_direction_input(GPIO_PWM0);
	panel_pwm_status = gpio_get_value(GPIO_PWM0);
}

/* Supported panels and dpi for nanopi4 series */
static char *panels[] = {
	"HD702E,213dpi",
	"HD101B,180dpi",
	"G101E,180dpi",
	"S701,160dpi",
	"HDMI1024x768,165dpi",
	"HDMI1280x800,168dpi",
};

char *board_get_panel_name(void)
{
	char *name;
	int i;

	name = getenv("panel");
	if (!name) {
		if (panel_pwm_status)
			printf("unknown eDP panal\n");
		return NULL;
	}

	for (i = 0; i < ARRAY_SIZE(panels); i++) {
		if (!strncmp(panels[i], name, strlen(name)))
			return panels[i];
	}

	return name;
}

int board_set_panel_name(const char *name)
{
	if (!panel_pwm_status && !getenv("panel"))
		setenv("panel", name);

	return 0;
}


/* RK3399 eFuse */
#define RK3399_A_SHIFT          16
#define RK3399_A_MASK           0x3ff
#define RK3399_NFUSES           32
#define RK3399_BYTES_PER_FUSE   4
#define RK3399_STROBSFTSEL      BIT(9)
#define RK3399_RSB              BIT(7)
#define RK3399_PD               BIT(5)
#define RK3399_PGENB            BIT(3)
#define RK3399_LOAD             BIT(2)
#define RK3399_STROBE           BIT(1)
#define RK3399_CSB              BIT(0)

#define RK3399_CPUID_OFF        0x7
#define RK3399_CPUID_LEN        0x10

static int rk3399_efuse_read(unsigned long base, int offset,
			     void *buf, int size)
{
	void *ctrl_reg = (uint32 *)base;
	void *dout_reg = (uint32 *)(base + 4);

	unsigned int addr_start, addr_end, addr_offset;
	u32 addr;
	u32 out_value;
	u8  bytes[RK3399_NFUSES * RK3399_BYTES_PER_FUSE];
	int i = 0;

	addr_start = offset / RK3399_BYTES_PER_FUSE;
	addr_offset = offset % RK3399_BYTES_PER_FUSE;
	addr_end = DIV_ROUND_UP(offset + size, RK3399_BYTES_PER_FUSE);

	/* cap to the size of the efuse block */
	if (addr_end > RK3399_NFUSES)
		addr_end = RK3399_NFUSES;

	writel(RK3399_LOAD | RK3399_PGENB | RK3399_STROBSFTSEL | RK3399_RSB,
	       ctrl_reg);

	udelay(1);
	for (addr = addr_start; addr < addr_end; addr++) {
		setbits_le32(ctrl_reg,
			     RK3399_STROBE | (addr << RK3399_A_SHIFT));
		udelay(1);
		out_value = readl(dout_reg);
		clrbits_le32(ctrl_reg, RK3399_STROBE);
		udelay(1);

		memcpy(&bytes[i], &out_value, RK3399_BYTES_PER_FUSE);
		i += RK3399_BYTES_PER_FUSE;
	}

	/* Switch to standby mode */
	writel(RK3399_PD | RK3399_CSB, ctrl_reg);

	memcpy(buf, bytes + addr_offset, size);
	return 0;
}

static void setup_serial(void)
{
	u8 cpuid[RK3399_CPUID_LEN];
	u8 low[RK3399_CPUID_LEN/2], high[RK3399_CPUID_LEN/2];
	char cpuid_str[RK3399_CPUID_LEN * 2 + 1];
	u64 serialno;
	char serialno_str[16];
	char *env_cpuid, *env_serial;
	int i;

	env_cpuid = getenv("cpuid#");
	env_serial = getenv("serial#");
	if (env_cpuid && env_serial)
		return;

	rk3399_efuse_read(RKIO_FTEFUSE_BASE, RK3399_CPUID_OFF, cpuid, sizeof(cpuid));

	memset(cpuid_str, 0, sizeof(cpuid_str));
	for (i = 0; i < 16; i++)
		sprintf(&cpuid_str[i * 2], "%02x", cpuid[i]);

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

	setenv("cpuid#", cpuid_str);
	if (!env_serial)
		setenv("serial#", serialno_str);
	saveenv();
}

static void setup_macaddr(void)
{
	int ret;
	const char *cpuid = getenv("cpuid#");
	u8 hash[SHA256_SUM_LEN];
	int size = sizeof(hash);
	uchar mac_addr[6];
	char buf[18];
	int i, n;

	/* Only generate a MAC address, if none is set in the environment */
	if (getenv("ethaddr"))
		return;

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

	for (i = 0, n = 0; i < sizeof(mac_addr); i++)
		n += sprintf(buf + n, "%02x:", mac_addr[i]);
	buf[17] = '\0';

	setenv("ethaddr", buf);
}


/*****************************************
 * Routine: board_init
 * Description: Early hardware init.
 *****************************************/
int board_init(void)
{
	/* Set Initial global variables */

	gd->bd->bi_arch_number = MACH_TYPE_RK30XX;
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x88000;

	return 0;
}


#ifdef CONFIG_DISPLAY_BOARDINFO
/**
 * Print board information
 */
int checkboard(void)
{
	puts("Board:\tRockchip platform Board\n");
#ifdef CONFIG_SECOND_LEVEL_BOOTLOADER
	printf("Uboot as second level loader\n");
#endif
	return 0;
}
#endif


#ifdef CONFIG_ARCH_EARLY_INIT_R
int arch_early_init_r(void)
{
	debug("arch_early_init_r\n");

	 /* set up exceptions */
	interrupt_init();
	/* enable exceptions */
	enable_interrupts();

	/* rk pl330 dmac init */
#ifdef CONFIG_RK_PL330_DMAC
	rk_pl330_dmac_init_all();
#endif /* CONFIG_RK_PL330_DMAC */

#ifdef CONFIG_RK_MCU
	rk_mcu_init();
#endif

	return 0;
}
#endif


#define RAMDISK_ZERO_COPY_SETTING	"0xffffffffffffffff=n\0"
static void board_init_adjust_env(void)
{
	bool change = false;

	char *s = getenv("bootdelay");
	if (s != NULL) {
		unsigned long bootdelay = 0;

		bootdelay = simple_strtoul(s, NULL, 16);
		debug("getenv: bootdelay = %lu\n", bootdelay);
#if (CONFIG_BOOTDELAY <= 0)
		if (bootdelay > 0) {
			setenv("bootdelay", simple_itoa(0));
			change = true;
			debug("setenv: bootdelay = 0\n");
		}
#else
		if (bootdelay != CONFIG_BOOTDELAY) {
			setenv("bootdelay", simple_itoa(CONFIG_BOOTDELAY));
			change = true;
			debug("setenv: bootdelay = %d\n", CONFIG_BOOTDELAY);
		}
#endif
	}

	s = getenv("bootcmd");
	if (s != NULL) {
		debug("getenv: bootcmd = %s\n", s);
		if (strcmp(s, CONFIG_BOOTCOMMAND) != 0) {
			setenv("bootcmd", CONFIG_BOOTCOMMAND);
			change = true;
			debug("setenv: bootcmd = %s\n", CONFIG_BOOTCOMMAND);
		}
	}

	s = getenv("initrd_high");
	if (s != NULL) {
		debug("getenv: initrd_high = %s\n", s);
		if (strcmp(s, RAMDISK_ZERO_COPY_SETTING) != 0) {
			setenv("initrd_high", RAMDISK_ZERO_COPY_SETTING);
			change = true;
			debug("setenv: initrd_high = %s\n", RAMDISK_ZERO_COPY_SETTING);
		}
	}

	if (change) {
#ifdef CONFIG_CMD_SAVEENV
		debug("board init saveenv.\n");
		saveenv();
#endif
	}
}


#ifdef CONFIG_BOARD_LATE_INIT
extern char bootloader_ver[24];
int board_late_init(void)
{
	debug("board_late_init\n");

	board_init_adjust_env();

	bd_hwrev_init();
	panel_pwm_status_init();
	set_dtb_name();

	setup_serial();
	setup_macaddr();

	load_disk_partitions();

#ifdef CONFIG_RK_PWM_REMOTE
        RemotectlInit();
#endif
	debug("rkimage_prepare_fdt\n");
	rkimage_prepare_fdt();

#ifdef CONFIG_RK_KEY
	debug("key_init\n");
	key_init();
#endif

#ifdef CONFIG_RK_POWER
	debug("fixed_init\n");
	fixed_regulator_init();
	debug("pmic_init\n");
	pmic_init(0);
#if defined(CONFIG_POWER_PWM_REGULATOR)
	debug("pwm_regulator_init\n");
	pwm_regulator_init();
#endif
	if (rkclk_set_apll_high())
		rkclk_dump_pll();
	debug("fg_init\n");
	fg_init(0); /*fuel gauge init*/
	debug("charger init\n");
	plat_charger_init();
#endif /* CONFIG_RK_POWER */

#ifdef CONFIG_OPTEE_CLIENT
       load_attestation_key();
#endif

	debug("idb init\n");
	//TODO:set those buffers in a better way, and use malloc?
	rkidb_setup_space(gd->arch.rk_global_buf_addr);

	/* after setup space, get id block data first */
	rkidb_get_idblk_data();

	/* Secure boot check after idb data get */
	SecureBootCheck();

	if (rkidb_get_bootloader_ver() == 0) {
		printf("\n#Boot ver: %s\n", bootloader_ver);
	}

	char tmp_buf[32];
	/* rk sn size 30bytes, zero buff */
	memset(tmp_buf, 0, 32);
	if (rkidb_get_sn(tmp_buf)) {
		setenv("fbt_sn#", tmp_buf);
	}

	debug("fbt preboot\n");
	board_fbt_preboot();

	return 0;
}
#endif

#ifdef CONFIG_ROCKCHIP_DISPLAY
extern void rockchip_display_fixup(void *blob);
#endif

#if defined(CONFIG_RK_DEVICEINFO)
extern bool g_is_devinfo_load;
#endif

#if defined(CONFIG_OF_LIBFDT) && defined(CONFIG_OF_BOARD_SETUP)
void ft_board_setup(void *blob, bd_t * bd)
{
#ifdef CONFIG_ROCKCHIP_DISPLAY
	rockchip_display_fixup(blob);
#endif
#ifdef CONFIG_ROCKCHIP
#if defined(CONFIG_LCD) || defined(CONFIG_VIDEO)
	u64 start, size;
	int offset;

	if (!gd->uboot_logo)
		return;

	start = gd->fb_base;
	offset = gd->fb_offset;
	if (offset > 0)
		size = CONFIG_RK_LCD_SIZE;
	else
		size = CONFIG_RK_FB_SIZE;

	fdt_update_reserved_memory(blob, "rockchip,fb-logo", start, size);

#if defined(CONFIG_RK_DEVICEINFO)
	if (g_is_devinfo_load)
		fdt_update_reserved_memory(blob, "rockchip,stb-devinfo",
					   CONFIG_RKHDMI_PARAM_ADDR,
					   SZ_8K);
#endif

#endif
#endif
}
#endif
