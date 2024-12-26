/*
 * Copyright 2017, Rockchip Electronics Co., Ltd
 * hisping lin, <hisping.lin@rock-chips.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef _OPTEE_CLIENT_RPC_H_
#define _OPTEE_CLIENT_RPC_H_

#include <optee_include/tee_base_types.h>
#include <optee_include/OpteeClientApiLib.h>

typedef struct{
	unsigned int Arg0;
	unsigned int Arg1;
	unsigned int Arg2;
	unsigned int Arg3;
	unsigned int Arg4;
	unsigned int Arg5;
	unsigned int Arg6;
	unsigned int Arg7;
} ARM_SMC_ARGS;

TEEC_Result OpteeRpcCallback(ARM_SMC_ARGS *ArmSmcArgs);

#endif /*_OPTEE_CLIENT_RPC_H_*/
