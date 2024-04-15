# Copyright 2018 Oticon A/S
# Copyright 2024 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

include make_inc/pre.mk

HW_SRCS_FILE=make_inc/54L15_hw_files
HAL_FLPR_SRCS_FILE=make_inc/54L15_flpr_hal_files
HAL_APP_SRCS_FILE=make_inc/54L15_app_hal_files

SRCS_HW=$(shell cat ${HW_SRCS_FILE})
SRCS_HAL_FLPR=$(shell cat ${HAL_FLPR_SRCS_FILE})
SRCS_HAL_APP=$(shell cat ${HAL_APP_SRCS_FILE})

ifneq (,$(findstring hal_flpr,$(MAKECMDGOALS)))
 SRCS:=${SRCS_HAL_FLPR}
 LIB_VARIANT:=_hal_flpr
 HAL_VARIANT=-DNRF54L15_XXAA -DNRF_FLPR
else ifneq (,$(findstring hal_app,$(MAKECMDGOALS)))
 SRCS:=${SRCS_HAL_APP}
 LIB_VARIANT:=_hal_app
 HAL_VARIANT=-DNRF54L15_XXAA -DNRF_APPLICATION
else #hw
 SRCS:=${SRCS_HW}
 LIB_VARIANT:=
 HAL_VARIANT=-DNRF54L15_XXAA -DNRF_APPLICATION
endif
VARIANT=NRF54L15${LIB_VARIANT}

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
          -I${NRFX_BASE}/mdk \
          -I${NRFX_BASE}/drivers/include

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
        -DNRF54L15 -D_XOPEN_SOURCE=500 ${HAL_VARIANT}
LDFLAGS:=${ARCH} ${COVERAGE}
CPPFLAGS:=

include make_inc/common_post.mk

.PHONY: hw hal_net hal_app

# Let's explicitly tell make there is rule to make this file 
54L15.mk: ;
