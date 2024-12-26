/*
 * Copyright 2017, Rockchip Electronics Co., Ltd
 * hisping lin, <hisping.lin@rock-chips.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <boot_rkimg.h>
#include <stdlib.h>
#include <command.h>
#include <mmc.h>
#include <optee_include/OpteeClientLoadTa.h>
#include <optee_include/OpteeClientMem.h>
#include <optee_include/OpteeClientRPC.h>
#include <optee_include/teesmc.h>
#include <optee_include/teesmc_v2.h>
#include <optee_include/teesmc_optee.h>
#include <optee_include/tee_mmc_rpmb.h>
#include <optee_include/tee_ufs_rpmb.h>
#include <optee_include/tee_rpc_types.h>
#include <optee_include/tee_rpc.h>
#ifdef CONFIG_OPTEE_V1
#include <optee_include/OpteeClientRkFs.h>
#endif
#ifdef CONFIG_OPTEE_V2
#include <optee_include/OpteeClientRkNewFs.h>
#endif

/*
 * Memory allocation.
 * Currently treated the same for both arguments & payloads.
 */
TEEC_Result OpteeRpcAlloc(uint32_t Size, uint32_t *Address)
{
	TEEC_Result TeecResult = TEEC_SUCCESS;
	size_t AllocAddress;

	*Address = 0;

	if (Size != 0) {
		AllocAddress = (size_t) OpteeClientMemAlloc(Size);

		if (AllocAddress == 0)
			TeecResult = TEEC_ERROR_OUT_OF_MEMORY;
		else
			*Address = AllocAddress;
	}
	return TeecResult;
}

/*
 * Memory free.
 * Currently treated the same for both arguments & payloads.
 */
TEEC_Result OpteeRpcFree(uint32_t Address)
{
	OpteeClientMemFree((void *)(size_t)Address);
	return TEEC_SUCCESS;
}

TEEC_Result OpteeRpcCmdLoadV2Ta(t_teesmc32_arg *TeeSmc32Arg)
{
	TEEC_Result TeecResult = TEEC_SUCCESS;
	t_teesmc32_param *TeeSmc32Param = NULL;
	int ta_found = 0;
	size_t size = 0;

	if (TeeSmc32Arg->num_params != 2) {
		TeecResult = TEEC_ERROR_BAD_PARAMETERS;
		goto Exit;
	}

	TeeSmc32Param = TEESMC32_GET_PARAMS(TeeSmc32Arg);

	size = TeeSmc32Param[1].u.memref.size;
	ta_found = search_ta((void *)(size_t)&TeeSmc32Param[0].u.value,
				(void *)(size_t)TeeSmc32Param[1].u.memref.buf_ptr, &size);
	if (ta_found == TA_BINARY_FOUND) {
		TeeSmc32Param[1].u.memref.size = size;
		TeecResult = TEEC_SUCCESS;
	} else {
		printf("  TA not found \n");
		TeecResult = TEEC_ERROR_ITEM_NOT_FOUND;
	}

Exit:
	TeeSmc32Arg->ret = TeecResult;
	TeeSmc32Arg->ret_origin = TEEC_ORIGIN_API;

	debug("TEEC: OpteeRpcCmdLoadV2Ta Exit : TeecResult=0x%X\n", TeecResult);

	return TeecResult;
}

/*
 * Execute an RPMB storage operation.
 */
TEEC_Result OpteeRpcCmdRpmb(t_teesmc32_arg *TeeSmc32Arg)
{
	struct blk_desc *dev_desc;

	dev_desc = rockchip_get_bootdev();
	if (!dev_desc) {
		printf("%s: dev_desc is NULL!\n", __func__);
		return TEEC_ERROR_GENERIC;
	}

	if (dev_desc->if_type == IF_TYPE_MMC && dev_desc->devnum == 0)//emmc
		return emmc_rpmb_process(TeeSmc32Arg);
	else if (dev_desc->if_type == IF_TYPE_SCSI)//ufs
		return ufs_rpmb_process(TeeSmc32Arg);

	printf("Device not support rpmb!\n");
	return TEEC_ERROR_NOT_IMPLEMENTED;
}

/*
 * Execute a normal world local file system operation.
 */
TEEC_Result OpteeRpcCmdFs(t_teesmc32_arg *TeeSmc32Arg)
{
	TEEC_Result TeecResult = TEEC_SUCCESS;
	t_teesmc32_param *TeeSmc32Param;

	TeeSmc32Param = TEESMC32_GET_PARAMS(TeeSmc32Arg);
#ifdef CONFIG_OPTEE_V1
	TeecResult = OpteeClientRkFsProcess((void *)(size_t)TeeSmc32Param[0].u.memref.buf_ptr,
							TeeSmc32Param[0].u.memref.size);
	TeeSmc32Arg->ret = TEEC_SUCCESS;
#endif
#ifdef CONFIG_OPTEE_V2
	TeecResult = OpteeClientRkFsProcess((size_t)TeeSmc32Arg->num_params,
							(struct tee_ioctl_param *)TeeSmc32Param);
	TeeSmc32Arg->ret = TeecResult;
#endif
	return TeecResult;
}

/*
 * TBD.
 */
TEEC_Result OpteeRpcCmdGetTime(t_teesmc32_arg *TeeSmc32Arg)
{
	return TEEC_ERROR_NOT_IMPLEMENTED;
}

/*
 * TBD.
 */
TEEC_Result OpteeRpcCmdWaitMutex(t_teesmc32_arg *TeeSmc32Arg)
{
	return TEEC_ERROR_NOT_IMPLEMENTED;
}

/*
 * Handle the callback from secure world.
 */
TEEC_Result OpteeRpcCallback(ARM_SMC_ARGS *ArmSmcArgs)
{
	TEEC_Result TeecResult = TEEC_SUCCESS;

	//printf("OpteeRpcCallback Enter: Arg0=0x%X, Arg1=0x%X, Arg2=0x%X\n",
		//ArmSmcArgs->Arg0, ArmSmcArgs->Arg1, ArmSmcArgs->Arg2);

	switch (TEESMC_RETURN_GET_RPC_FUNC(ArmSmcArgs->Arg0)) {
	case TEESMC_RPC_FUNC_ALLOC_ARG: {
		debug("TEEC: ArmSmcArgs->Arg1 = 0x%x \n", ArmSmcArgs->Arg1);
		TeecResult = OpteeRpcAlloc(ArmSmcArgs->Arg1, &ArmSmcArgs->Arg2);
		ArmSmcArgs->Arg5 = ArmSmcArgs->Arg2;
		ArmSmcArgs->Arg1 = 0;
		ArmSmcArgs->Arg4 = 0;
		break;
	}

	case TEESMC_RPC_FUNC_ALLOC_PAYLOAD: {
		TeecResult = OpteeRpcAlloc(ArmSmcArgs->Arg1, &ArmSmcArgs->Arg1);
		break;
	}

	case TEESMC_RPC_FUNC_FREE_ARG: {
		TeecResult = OpteeRpcFree(ArmSmcArgs->Arg2);
		break;
	}

	case TEESMC_RPC_FUNC_FREE_PAYLOAD: {
		TeecResult = OpteeRpcFree(ArmSmcArgs->Arg1);
		break;
	}

	case TEESMC_RPC_FUNC_IRQ: {
		break;
	}

	case TEESMC_RPC_FUNC_CMD: {
		t_teesmc32_arg *TeeSmc32Arg =
			(t_teesmc32_arg *)(size_t)((uint64_t)ArmSmcArgs->Arg1 << 32 | ArmSmcArgs->Arg2);
		debug("TEEC: TeeSmc32Arg->cmd = 0x%x\n", TeeSmc32Arg->cmd);
		switch (TeeSmc32Arg->cmd) {
		case OPTEE_MSG_RPC_CMD_SHM_ALLOC_V2: {
			uint32_t tempaddr;
			uint32_t allocsize = TeeSmc32Arg->params[0].u.value.b;
			TeecResult = OpteeRpcAlloc(allocsize, &tempaddr);
			debug("TEEC: allocsize = 0x%x tempaddr = 0x%x\n", allocsize, tempaddr);
			TeeSmc32Arg->params[0].attr = OPTEE_MSG_ATTR_TYPE_TMEM_OUTPUT_V2;
			TeeSmc32Arg->params[0].u.memref.buf_ptr = tempaddr;
			TeeSmc32Arg->params[0].u.memref.size = allocsize;
			TeeSmc32Arg->params[0].u.memref.shm_ref = tempaddr;
			TeeSmc32Arg->ret = TEE_SUCCESS;
			break;
		}
		case OPTEE_MSG_RPC_CMD_SHM_FREE_V2: {
			uint32_t tempaddr = TeeSmc32Arg->params[0].u.value.b;
			TeecResult = OpteeRpcFree(tempaddr);
			break;

		}
		case OPTEE_MSG_RPC_CMD_RPMB_V2: {
			TeecResult = OpteeRpcCmdRpmb(TeeSmc32Arg);
			break;
		}
		case OPTEE_MSG_RPC_CMD_FS_V2: {
			TeecResult = OpteeRpcCmdFs(TeeSmc32Arg);
			break;
		}
		case OPTEE_MSG_RPC_CMD_LOAD_TA_V2: {
			TeecResult = OpteeRpcCmdLoadV2Ta(TeeSmc32Arg);
			break;
		}

		default: {
			printf("TEEC: ...unsupported RPC CMD: cmd=0x%X\n",
				TeeSmc32Arg->cmd);
			TeecResult = TEEC_ERROR_NOT_IMPLEMENTED;
			break;
		}
	}

		break;
	}

	case TEESMC_OPTEE_RPC_FUNC_ALLOC_PAYLOAD: {
		TeecResult = OpteeRpcAlloc(ArmSmcArgs->Arg1, &ArmSmcArgs->Arg1);
		ArmSmcArgs->Arg2 = ArmSmcArgs->Arg1;
		break;
	}

	case TEESMC_OPTEE_RPC_FUNC_FREE_PAYLOAD: {
		TeecResult = OpteeRpcFree(ArmSmcArgs->Arg1);
		break;
	}

	default: {
		printf("TEEC: ...unsupported RPC : Arg0=0x%X\n", ArmSmcArgs->Arg0);
		TeecResult = TEEC_ERROR_NOT_IMPLEMENTED;
		break;
	}
	}

	ArmSmcArgs->Arg0 = TEESMC32_CALL_RETURN_FROM_RPC;
	debug("TEEC: OpteeRpcCallback Exit : TeecResult=0x%X\n", TeecResult);

	return TeecResult;
}
