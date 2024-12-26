/*
 * Copyright 2024, Rockchip Electronics Co., Ltd
 * hisping lin, <hisping.lin@rock-chips.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <stdlib.h>
#include <command.h>
#include <optee_include/OpteeClientRPC.h>
#include <optee_include/tee_rpc.h>
#include <optee_include/tee_rpc_types.h>
#include <optee_include/teesmc_v2.h>
#include <optee_include/tee_client_api.h>
#include "../../drivers/ufs/ufs.h"
#include "../../drivers/ufs/ufs-rockchip-rpmb.h"

/*
 * Execute an RPMB storage operation.
 */
#ifdef CONFIG_ROCKCHIP_UFS_RPMB

static uint16_t bedata_to_u16(uint8_t *d)
{
	return (d[0] << 8) + (d[1]);
}

static uint64_t bedata_to_u64(uint8_t *d)
{
	uint64_t data = 0;
	uint64_t temp = 0;

	for (int i = 0; i < 8; i++) {
		temp = d[i];
		data += (temp << ((7 - i) * 8));
	}

	return data;
}

static void u16_to_bedata(uint16_t src, uint8_t *d)
{
	d[0] = (src >> 8) & 0xff;
	d[1] = src & 0xff;
}

static int rpmb_data_req(struct rpmb_data_frame *req_frm,
			 size_t req_nfrm,
			 struct rpmb_data_frame *rsp_frm,
			 size_t rsp_nfrm)
{
	struct rpmb_data_frame *req_packets = NULL;
	struct rpmb_data_frame *rsp_packets = NULL;
	uint16_t req_type;
	int ret = -1;

	req_packets = memalign(CONFIG_SYS_CACHELINE_SIZE, sizeof(struct rpmb_data_frame) * req_nfrm);
	if (!req_packets)
		goto out;

	rsp_packets = memalign(CONFIG_SYS_CACHELINE_SIZE, sizeof(struct rpmb_data_frame) * rsp_nfrm);
	if (!rsp_packets)
		goto out;

	memcpy(req_packets, req_frm, sizeof(struct rpmb_data_frame) * req_nfrm);

	req_type = bedata_to_u16(req_packets->msg_type);

	switch (req_type) {
	case TEE_RPC_RPMB_MSG_TYPE_REQ_AUTH_KEY_PROGRAM:
	case TEE_RPC_RPMB_MSG_TYPE_REQ_AUTH_DATA_WRITE: {
		ret = do_rpmb_op(req_packets, req_nfrm, rsp_packets, rsp_nfrm);
		break;
	}
	case TEE_RPC_RPMB_MSG_TYPE_REQ_WRITE_COUNTER_VAL_READ: {
		do_rpmb_op(req_packets, req_nfrm, rsp_packets, rsp_nfrm);
		ret = TEEC_SUCCESS;
		break;
	}
	case TEE_RPC_RPMB_MSG_TYPE_REQ_AUTH_DATA_READ: {
		uint16_t block_count;
		block_count = bedata_to_u16(req_packets->block_count);
		if (block_count == 0) {
			u16_to_bedata(rsp_nfrm, req_packets->block_count);
		}
		ret = do_rpmb_op(req_packets, req_nfrm, rsp_packets, rsp_nfrm);
		break;
	}
	default:
		ret = TEEC_ERROR_BAD_PARAMETERS;
		break;
	}

	for (int i = 0; i < rsp_nfrm; i++)
		memcpy(rsp_frm + i, rsp_packets + i, sizeof(struct rpmb_data_frame));

out:
	if (req_packets)
		free(req_packets);

	if (rsp_packets)
		free(rsp_packets);

	return ret;
}

static int rpmb_get_dev_info(struct tee_rpc_rpmb_dev_info *info)
{
	uint8_t manufacture_name_idx;
	uint8_t rpmb_rw_blocks;
	uint64_t rpmb_block_count;
	uint8_t desc_buff[QUERY_DESC_MAX_SIZE] = { 0 };
	uint8_t str_buff[QUERY_DESC_MAX_SIZE] = { 0 };
	uint8_t geo_buff[QUERY_DESC_MAX_SIZE] = { 0 };
	uint8_t unit_buf[QUERY_DESC_MAX_SIZE] = { 0 };

	if (ufs_read_device_desc(desc_buff, sizeof(desc_buff))) {
		printf("get device desc fail!\n");
		return -1;
	}
	manufacture_name_idx = desc_buff[0x14];

	if (ufs_read_string_desc(manufacture_name_idx, str_buff, sizeof(str_buff))) {
		printf("get device desc fail!\n");
		return -1;
	}

	if (str_buff[0] != 0x12) {
		printf("manufacture name string length error!\n");
		return -1;
	}

	if (sizeof(info->cid) != 0x10) {
		printf("cid length error!\n");
		return -1;
	}

	memcpy(info->cid, &str_buff[2], sizeof(info->cid));

	if (ufs_read_geo_desc(geo_buff, sizeof(geo_buff))) {
		printf("get geometry desc fail!\n");
		return -1;
	}
	rpmb_rw_blocks = geo_buff[0x17];

	if (ufs_read_rpmb_unit_desc(unit_buf, sizeof(unit_buf))) {
		printf("get rpmb ubit desc fail!\n");
		return -1;
	}
	rpmb_block_count = bedata_to_u64(&unit_buf[0x0B]);

	info->rel_wr_sec_c = rpmb_rw_blocks;
	info->rpmb_size_mult = rpmb_block_count / 512;
	info->ret_code = 0;

	return 0;
}

TEEC_Result ufs_rpmb_process(t_teesmc32_arg *smc_arg)
{
	t_teesmc32_param *smc_param;
	struct tee_rpc_rpmb_cmd *rpmb_req;
	struct rpmb_data_frame *req_packets = NULL;
	struct rpmb_data_frame *rsp_packets = NULL;
	size_t req_num, rsp_num;
	struct tee_rpc_rpmb_dev_info *dev_info;
	TEEC_Result result = TEEC_SUCCESS;
	TEEC_Result status;

	if (smc_arg->num_params != 2) {
		result = TEEC_ERROR_BAD_PARAMETERS;
		goto exit;
	}

	smc_param = TEESMC32_GET_PARAMS(smc_arg);
	rpmb_req = (struct tee_rpc_rpmb_cmd *)(size_t)
		   smc_param[0].u.memref.buf_ptr;

	switch (rpmb_req->cmd) {
	case TEE_RPC_RPMB_CMD_DATA_REQ: {
		req_packets = (struct rpmb_data_frame *)(rpmb_req + 1);
		rsp_packets = (struct rpmb_data_frame *)(size_t)smc_param[1].u.memref.buf_ptr;
		req_num = bedata_to_u16(req_packets->block_count);
		req_num = (req_num == 0 ? 1 : req_num);
		rsp_num = (rpmb_req->block_count == 0 ? 1 : rpmb_req->block_count);

		status = rpmb_data_req(req_packets, req_num, rsp_packets, rsp_num);
		if (status != 0)
			result = TEEC_ERROR_GENERIC;
		goto exit;
	}
	case TEE_RPC_RPMB_CMD_GET_DEV_INFO: {
		dev_info = (struct tee_rpc_rpmb_dev_info *)
			   (size_t)smc_param[1].u.memref.buf_ptr;

		status = rpmb_get_dev_info(dev_info);
		if (status != 0)
			result = TEEC_ERROR_GENERIC;
		goto exit;
	}
	default:
		result = TEEC_ERROR_BAD_PARAMETERS;
		goto exit;
	}

exit:
	smc_arg->ret = result;
	smc_arg->ret_origin = TEEC_ORIGIN_API;

	return result;
}

#else

TEEC_Result ufs_rpmb_process(t_teesmc32_arg *smc_arg)
{
	smc_arg = smc_arg;
	return TEEC_ERROR_NOT_IMPLEMENTED;
}

#endif
