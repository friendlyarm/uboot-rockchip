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

#include <asm/io.h>
#include <asm/arch/rkplat.h>

#include "../common/config.h"
#ifdef CONFIG_OPTEE_CLIENT
#include "../common/rkloader/attestation_key.h"
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

#define SARADC_BASE		RKIO_SARADC_BASE
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
 * Board revision list: <GPIO2_B4 | GPIO4_A3>
 *  0b10 - NanoPi-R2
 *
 * Extended by ADC_IN1
 */
static int pcb_rev = -1;

static void bd_hwrev_init(void)
{
	int idx;

	gpio_direction_input(GPIO_BANK2 | GPIO_A3);
	gpio_direction_input(GPIO_BANK2 | GPIO_B4);

	pcb_rev  =  gpio_get_value(GPIO_BANK2 | GPIO_A3) << 4;
	pcb_rev |= (gpio_get_value(GPIO_BANK2 | GPIO_B4) << 5);

	idx = get_adc_index(1);
	if (idx > 0)
		pcb_rev += idx;
}

/* To override __weak symbols */
u32 get_board_rev(void)
{
	return pcb_rev;
}

static void set_dtb_name(void)
{
	char info[64] = {0, };

	if (getenv("dtb_name"))
		return;

	snprintf(info, ARRAY_SIZE(info),
			"rk3328-nanopi-r2-rev%02x.dtb", get_board_rev());
	setenv("dtb_name", info);
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
	set_dtb_name();

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
