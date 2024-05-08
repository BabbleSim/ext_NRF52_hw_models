# Copyright 2023 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

ifndef NRFX_BASE
$(error NRFX_BASE must be set to the nrfx checkout folder)
endif

ifndef NATIVE_SIM_PATH
$(error NATIVE_SIM_PATH must be set to the native_simulator checkout folder)
endif

default: NRF52833 NRF5340 NRF54L15
compile: NRF52833

NRF52833:
	@$(MAKE) --no-print-directory -f 52833.mk

NRF5340:
	@$(MAKE) --no-print-directory -f 5340.mk hw
	@$(MAKE) --no-print-directory -f 5340.mk hal_net
	@$(MAKE) --no-print-directory -f 5340.mk hal_app

NRF5340_hal_net:
	@$(MAKE) --no-print-directory -f 5340.mk hw
	@$(MAKE) --no-print-directory -f 5340.mk hal_net

NRF5340_hal_app:
	@$(MAKE) --no-print-directory -f 5340.mk hw
	@$(MAKE) --no-print-directory -f 5340.mk hal_app

NRF54L15:
	@$(MAKE) --no-print-directory -f 54L15.mk hw
	@$(MAKE) --no-print-directory -f 54L15.mk hal_app

# Let's just let the 52833 build handle any other target by default
%::
	@$(MAKE) -f 52833.mk $@

.PHONY: NRF52833 NRF5340 NRF5340_hal_net NRF5340_hal_app NRF54L15 default compile

# No need to check implicit rules for this file itself
Makefile: ;
