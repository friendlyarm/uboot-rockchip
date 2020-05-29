#!/bin/bash
#
# Copyright (c) 2020 Fuzhou Rockchip Electronics Co., Ltd
#
# SPDX-License-Identifier: GPL-2.0
#
set -e

# openssl dgst -sha256 -sign keys/dev.key -out sha256-rsa2048.sign fit/boot.data2sign

source scripts/fit-base.sh
fit_resign $*
