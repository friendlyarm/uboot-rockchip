/*
 * (C) Copyright 2002-2008
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __UBOOT_FW_ENV_H__
#define __UBOOT_FW_ENV_H__		1

/* Pull in the current config to define the default environment */
#ifndef __ASSEMBLY__
#define __ASSEMBLY__ /* get only #defines from config.h */
#include <config.h>
#undef	__ASSEMBLY__
#else
#include <config.h>
#endif

/*
 * To build the utility with the static configuration
 * comment out the next line.
 * See included "fw_env.config" sample file
 * for notes on configuration.
 */
#define CONFIG_FILE     "/etc/fw_env.config"

#ifndef CONFIG_FILE
#define HAVE_REDUND /* For systems with 2 env sectors */
#define DEVICE1_NAME      "/dev/mtd1"
#define DEVICE2_NAME      "/dev/mtd2"
#define DEVICE1_OFFSET    0x0000
#define ENV1_SIZE         0x4000
#define DEVICE1_ESIZE     0x4000
#define DEVICE1_ENVSECTORS     2
#define DEVICE2_OFFSET    0x0000
#define ENV2_SIZE         0x4000
#define DEVICE2_ESIZE     0x4000
#define DEVICE2_ENVSECTORS     2
#endif

#ifdef CONFIG_ENV_IS_IN_RK_STORAGE
#undef  CONFIG_ENV_OFFSET
#undef  CONFIG_ENV_SIZE
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_ENV_OFFSET (0x1FC0 * 512)
#define CONFIG_ENV_SIZE   (0x0040 * 512)
#endif

#ifdef CONFIG_ENV_IS_IN_MMC
#undef  CONFIG_FILE
#undef  HAVE_REDUND
#define DEVICE1_NAME      "/dev/mmcblk0"
#define DEVICE1_OFFSET    CONFIG_ENV_OFFSET
#define ENV1_SIZE         CONFIG_ENV_SIZE
#endif

#ifndef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE   115200
#endif

#ifndef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY  5 /* autoboot after 5 seconds */
#endif

#ifndef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND							\
	"bootp; "								\
	"setenv bootargs root=/dev/nfs nfsroot=${serverip}:${rootpath} "	\
	"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}::off; "	\
	"bootm"
#endif

extern int   fw_printenv(int argc, char *argv[]);
extern char *fw_getenv  (char *name);
extern int fw_setenv  (int argc, char *argv[]);
extern int fw_parse_script(char *fname);
extern int fw_set_device(const char *dname);
extern int fw_env_open(void);
extern int fw_env_write(char *name, char *value);
extern int fw_env_close(void);

extern unsigned long crc32 (unsigned long, const unsigned char *, unsigned);

#endif /* __UBOOT_FW_ENV_H__ */
