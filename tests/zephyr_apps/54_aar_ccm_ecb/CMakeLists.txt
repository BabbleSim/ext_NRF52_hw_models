# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(aar_ccm_ecb_test)

target_sources(app PRIVATE
  src/test_ccm.c
  src/test_ecb.c
)
