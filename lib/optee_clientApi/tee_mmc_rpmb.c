/*
 * Copyright 2024, Rockchip Electronics Co., Ltd
 * hisping lin, <hisping.lin@rock-chips.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <stdlib.h>
#include <command.h>
#include <mmc.h>
#include <optee_include/OpteeClientRPC.h>
#include <optee_include/tee_rpc.h>
#include <optee_include/tee_rpc_types.h>
#include <optee_include/teesmc_v2.h>
#include <optee_include/tee_client_api.h>

/*
 * Execute an RPMB storage operation.
 */
#ifdef CONFIG_SUPPORT_EMMC_RPMB

static int rpmb_data_req(struct s_rpmb *req_frm,
			 size_t req_nfrm,
			 struct s_rpmb *rsp_frm,
			 size_t rsp_nfrm)
{
	struct s_rpmb *req_packets = NULL;
	uint16_t req_type;
	int ret;

	req_packets = memalign(CONFIG_SYS_CACHELINE_SIZE, sizeof(struct s_rpmb) * (req_nfrm + rsp_nfrm));
	if (!req_packets)
		return -1;

	memcpy(req_packets, req_frm, sizeof(struct s_rpmb) * req_nfrm);

	req_type = cpu_to_be16(req_packets->request);

	switch (req_type) {
	case TEE_RPC_RPMB_MSG_TYPE_REQ_AUTH_KEY_PROGRAM: {
		ret = do_programkey(req_packets);
		break;
	}
	case TEE_RPC_RPMB_MSG_TYPE_REQ_WRITE_COUNTER_VAL_READ: {
		do_readcounter(req_packets);
		ret = TEEC_SUCCESS;
		break;
	}
	case TEE_RPC_RPMB_MSG_TYPE_REQ_AUTH_DATA_WRITE: {
		ret = do_authenticatedwrite(req_packets);
		break;
	}
	case TEE_RPC_RPMB_MSG_TYPE_REQ_AUTH_DATA_READ: {
		ret = do_authenticatedread(req_packets, rsp_nfrm);
		break;
	}
	default:
		ret = TEEC_ERROR_BAD_PARAMETERS;
		break;
	}

	for (int i = 0; i < rsp_nfrm; i++)
		memcpy(rsp_frm + i, req_packets + i, sizeof(struct s_rpmb));

	if (req_packets)
		free(req_packets);

	return ret;
}

static int rpmb_get_dev_info(struct tee_rpc_rpmb_dev_info *info)
{
	struct mmc *mmc;
	uint32_t cid_val[4];

	mmc = do_returnmmc();
	if (!mmc)
		return -1;

	for (int i = 0; i < 4; i++)
		cid_val[i] = cpu_to_be32(mmc->cid[i]);

	memcpy(info->cid, cid_val, sizeof(info->cid));

	info->rel_wr_sec_c = 1;
	info->rpmb_size_mult = (uint8_t)(mmc->capacity_rpmb / (128 * 1024));
	info->ret_code = 0;

	return 0;
}

TEEC_Result emmc_rpmb_process(t_teesmc32_arg *smc_arg)
{
	t_teesmc32_param *smc_param;
	struct tee_rpc_rpmb_cmd *rpmb_req;
	struct s_rpmb *req_packets = NULL;
	struct s_rpmb *rsp_packets = NULL;
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
		req_packets = (struct s_rpmb *)(rpmb_req + 1);
		rsp_packets = (struct s_rpmb *)(size_t)smc_param[1].u.memref.buf_ptr;
		req_num = cpu_to_be16(req_packets->block_count);
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

TEEC_Result emmc_rpmb_process(t_teesmc32_arg *smc_arg)
{
	smc_arg = smc_arg;
	return TEEC_ERROR_NOT_IMPLEMENTED;
}

#endif
