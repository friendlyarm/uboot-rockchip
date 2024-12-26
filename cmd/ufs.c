// SPDX-License-Identifier: GPL-2.0+
/**
 * ufs.c - UFS specific U-boot commands
 *
 * Copyright (C) 2019 Texas Instruments Incorporated - http://www.ti.com
 *
 */
#include <common.h>
#include <command.h>
#include <console.h>
#include <ufs.h>

#ifdef CONFIG_ROCKCHIP_UFS_RPMB
int ufs_rpmb_blk_read(char *blk_data, uint8_t *key, uint16_t blk_index, uint16_t block_count);
int ufs_rpmb_blk_write(char *write_data, uint8_t *key, uint16_t blk_index, uint16_t blk_count);
int check_ufs_rpmb_key(uint8_t *package, int package_size);
int ufs_rpmb_write_key(uint8_t * key, uint8_t len);
uint32_t ufs_rpmb_read_writecount(void);
/* @retrun 0 rpmb key unwritten */
int is_wr_ufs_rpmb_key(void);
int prepare_rpmb_lu(void);

#define RPMB_KEY_SIZE 32
char rpmb_test_key[RPMB_KEY_SIZE] =
{
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};

static int confirm_key_prog(char *key_addr)
{
	int i;

	printf("\nProgram key data is:\n");
	for (i = 0; i < 32; i++) {
		printf("%2x ", key_addr[i]);
		if (i == 15)
			printf("\n");
	}

	puts("\nWarning: Programming authentication key can be done only once !\n"
	     "         Use this command only if you are sure of what you are doing,\n"
	     "Really perform the key programming? <y/N> ");
	if (confirm_yesno())
		return 1;

	puts("Authentication key programming aborted\n");
	return 0;
}

static int do_ufs_rpmb_key(cmd_tbl_t *cmdtp, int flag,
			  int argc, char * const argv[])
{
	void *key_addr;

	if (argc != 2)
		return CMD_RET_USAGE;

	key_addr = (void *)simple_strtoul(argv[1], NULL, 16);
	if (!key_addr)
		key_addr = rpmb_test_key;

	if (!confirm_key_prog(key_addr))
		return CMD_RET_FAILURE;

	if (ufs_rpmb_write_key(key_addr, RPMB_KEY_SIZE)) {
		printf("ERROR - Key already programmed ?\n");
		return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}

static int do_ufs_rpmb_read(cmd_tbl_t *cmdtp, int flag,
			   int argc, char * const argv[])
{
	u16 blk, cnt;
	void *addr;
	int n;
	void *key_addr = NULL;

	if (argc < 4)
		return CMD_RET_USAGE;

	addr = (void *)simple_strtoul(argv[1], NULL, 16);
	blk = simple_strtoul(argv[2], NULL, 16);
	cnt = simple_strtoul(argv[3], NULL, 16);

	if (argc == 5) {
		key_addr = (void *)simple_strtoul(argv[4], NULL, 16);
		if (!key_addr)
			key_addr = rpmb_test_key;
	}

	printf("\nMMC RPMB read: block # %d, count %d ... \n", blk, cnt);
	n =  ufs_rpmb_blk_read(addr, key_addr, blk, cnt);

	printf("%d RPMB blocks read: %s\n", n, (n == cnt) ? "OK" : "ERROR");
	if (n != cnt)
		return CMD_RET_FAILURE;

	return CMD_RET_SUCCESS;
}

static int do_ufs_rpmb_write(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u16 blk, cnt;
	void *addr;
	int n;
	void *key_addr;

	if (argc != 5)
		return CMD_RET_USAGE;

	addr = (void *)simple_strtoul(argv[1], NULL, 16);
	blk = simple_strtoul(argv[2], NULL, 16);
	cnt = simple_strtoul(argv[3], NULL, 16);
	key_addr = (void *)simple_strtoul(argv[4], NULL, 16);
	if (!key_addr)
		key_addr = rpmb_test_key;
	printf("\nUFS RPMB write: block # %d, count %d ...\n", blk, cnt);

	n = ufs_rpmb_blk_write(addr, key_addr, blk, cnt);

	printf("%d RPMB blocks written: %s\n", n, (n == cnt) ? "OK" : "ERROR");
	if (n != cnt)
		return CMD_RET_FAILURE;

	return CMD_RET_SUCCESS;
}

static int do_ufs_rpmb_counter(cmd_tbl_t *cmdtp, int flag,
			      int argc, char * const argv[])
{
	uint32_t counter;

	counter = ufs_rpmb_read_writecount();
	printf("RPMB Write counter= %x\n", counter);

	return CMD_RET_SUCCESS;
}

static cmd_tbl_t cmd_rpmb[] = {
	U_BOOT_CMD_MKENT(key, 2, 0, do_ufs_rpmb_key, "", ""),
	U_BOOT_CMD_MKENT(read, 5, 1, do_ufs_rpmb_read, "", ""),
	U_BOOT_CMD_MKENT(write, 5, 0, do_ufs_rpmb_write, "", ""),
	U_BOOT_CMD_MKENT(counter, 1, 1, do_ufs_rpmb_counter, "", ""),
};

static int do_ufs_rpmb(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *cp;

	cp = find_cmd_tbl(argv[1], cmd_rpmb, ARRAY_SIZE(cmd_rpmb));

	/* Drop the rpmb subcommand */
	argc--;
	argv++;

	if (cp == NULL || argc > cp->maxargs)
		return CMD_RET_USAGE;

	if (flag == CMD_FLAG_REPEAT && !cp->repeatable)
		return CMD_RET_SUCCESS;

	return cp->cmd(cmdtp, flag, argc, argv);
}
#endif

static int do_ufs(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int dev, ret;

	if (argc >= 2) {
		if (!strcmp(argv[1], "init")) {
			if (argc == 3) {
				dev = simple_strtoul(argv[2], NULL, 10);
				ret = ufs_probe_dev(dev);
				if (ret)
					return CMD_RET_FAILURE;
			} else {
				ufs_probe();
			}

			return CMD_RET_SUCCESS;
		}
#ifdef CONFIG_ROCKCHIP_UFS_RPMB
		else if (!strcmp(argv[1], "rpmb")){
			/* Drop the ufs subcommand */
			argc--;
			argv++;

			return do_ufs_rpmb(cmdtp, flag, argc, argv);
		}
#endif
	}

	return CMD_RET_USAGE;
}

U_BOOT_CMD(ufs, 7, 1, do_ufs,
	   "UFS  sub system",
	   "init [dev] - init UFS subsystem\n"
#ifdef CONFIG_ROCKCHIP_UFS_RPMB
	   "ufs rpmb read addr blk# cnt [address of auth-key] - block size is 256 bytes\n"
	   "ufs rpmb write addr blk# cnt <address of auth-key> - block size is 256 bytes\n"
	   "ufs rpmb key <address of auth-key> - program the RPMB authentication key.\n"
	   "ufs rpmb counter - read the value of the write counter\n"
#endif
);
