# Copyright 2018 Oticon A/S
# SPDX-License-Identifier: Apache-2.0

BSIM_BASE_PATH?=$(abspath ../ )
include ${BSIM_BASE_PATH}/common/pre.make.inc

2G4_libPhyComv1_COMP_PATH?=$(abspath ${BSIM_COMPONENTS_PATH}/ext_2G4_libPhyComv1)

SRCS:=$(shell find src/ -name "*.c")

ifndef NRFX_BASE
$(error NRFX_BASE must be set to the nrfx checkout folder)
endif

INCLUDES:=-I${libUtilv1_COMP_PATH}/src/ \
          -I${libPhyComv1_COMP_PATH}/src/ \
          -I${2G4_libPhyComv1_COMP_PATH}/src \
          -I${libRandv2_COMP_PATH}/src/ \
          -Isrc/nrfx/mdk_replacements \
          -Isrc/HW_models/ \
          -Isrc/nrfx_config \
          -Isrc/nrfx/nrfx_replacements \
          -I${NRFX_BASE} \
          -I${NRFX_BASE}/mdk


LIB_NAME:=libNRF52_hw_models.32
A_LIBS:=
A_LIBS32:=
SO_LIBS:=
DEBUG:=-g
OPT:=
ARCH:=-m32
WARNINGS:=-Wall -pedantic
COVERAGE:=
CFLAGS:=${ARCH} ${DEBUG} ${OPT} ${WARNINGS} -MMD -MP -std=c11 \
        ${INCLUDES} -fdata-sections -ffunction-sections \
        -DNRF52832_XXAA
LDFLAGS:=${ARCH} ${COVERAGE}
CPPFLAGS:=

include ${BSIM_BASE_PATH}/common/make.lib_a.inc
