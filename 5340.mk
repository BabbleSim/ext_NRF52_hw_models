# Copyright 2018 Oticon A/S
# Copyright 2023 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

include make_inc/pre.mk

HW_SRCS_FILE=make_inc/5340_hw_files
HAL_NET_SRCS_FILE=make_inc/5340_net_hal_files
HAL_APP_SRCS_FILE=make_inc/5340_app_hal_files

SRCS_HW=$(shell cat ${HW_SRCS_FILE})
SRCS_HAL_NET=$(shell cat ${HAL_NET_SRCS_FILE})
SRCS_HAL_APP=$(shell cat ${HAL_APP_SRCS_FILE})

ifneq (,$(findstring hal_net,$(MAKECMDGOALS)))
 SRCS:=${SRCS_HAL_NET}
 LIB_VARIANT:=_hal_net
 HAL_VARIANT=NRF5340_XXAA_NETWORK
else ifneq (,$(findstring hal_app,$(MAKECMDGOALS)))
 SRCS:=${SRCS_HAL_APP}
 LIB_VARIANT:=_hal_app
 HAL_VARIANT=NRF5340_XXAA_APPLICATION
else #hw
 SRCS:=${SRCS_HW}
 LIB_VARIANT:=
 HAL_VARIANT=NRF5340_XXAA_NETWORK
endif
VARIANT=NRF5340${LIB_VARIANT}

hw: all
hal_net: all
hal_app: all

INCLUDES:=-I${NATIVE_SIM_PATH}/common/src/include/ \
          -I${NATIVE_SIM_PATH}/common/src/ \
          -I${libUtilv1_COMP_PATH}/src/ \
          -I${libPhyComv1_COMP_PATH}/src/ \
          -I${2G4_libPhyComv1_COMP_PATH}/src \
          -I${libRandv2_COMP_PATH}/src/ \
          -Isrc/nrfx/mdk_replacements \
          -Isrc/HW_models/ \
          -Isrc/nrfx_config \
          -Isrc/nrfx/nrfx_replacements \
          -I${NRFX_BASE} \
          -I${NRFX_BASE}/mdk

NRF_HW_MODELS_LIB_SUFFIX?=
LIB_NAME=libnRF_hw_models.${VARIANT}.32${NRF_HW_MODELS_LIB_SUFFIX}
A_LIBS:=
A_LIBS32:=
SO_LIBS:=
DEBUG:=-g
OPT:=
ARCH:=-m32
WARNINGS:=-Wall -Wpedantic
COVERAGE:=
CFLAGS=${ARCH} ${DEBUG} ${OPT} ${WARNINGS} -MMD -MP -std=gnu11 \
        ${INCLUDES} -fdata-sections -ffunction-sections \
        -DNRF5340 -D_XOPEN_SOURCE=500 -D${HAL_VARIANT}
LDFLAGS:=${ARCH} ${COVERAGE}
CPPFLAGS:=

include make_inc/common_post.mk

.PHONY: hw hal_net hal_app

# Let's explicitly tell make there is rule to make this file 
5340.mk: ;
