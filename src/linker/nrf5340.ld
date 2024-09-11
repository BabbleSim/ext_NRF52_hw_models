/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

SECTIONS
{
	virt_RAM.appcore_RAM 0x20000000 :
	{
		KEEP(*(virt_RAM.appcore_RAM));
	}
} INSERT AFTER .data;
