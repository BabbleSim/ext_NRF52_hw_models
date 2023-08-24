# Copyright 2018 Oticon A/S
# Copyright 2023 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

include make_inc/pre.mk

VARIANT=NRF52833
HAL_VARIANT=NRF52833_XXAA

HW_SRCS_FILE=make_inc/52833_hw_files
HAL_SRCS_FILE=make_inc/52833_hal_files

SRCS_HW=$(shell cat ${HW_SRCS_FILE})
SRCS_HAL=$(shell cat ${HAL_SRCS_FILE})

SRCS=${SRCS_HW} ${SRCS_HAL}

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
        -D${VARIANT} -D_XOPEN_SOURCE=500 -D${HAL_VARIANT}
LDFLAGS:=${ARCH} ${COVERAGE}
CPPFLAGS:=

include make_inc/common_post.mk
