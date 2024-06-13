/*
 * Copyright (c) 2018 Oticon A/S
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>

void hwu_reverse_byte_order(const unsigned char *in_data, unsigned char *out_data, size_t len)
{
    int i;
    in_data += len - 1;
    for (i = 0; i < len; i++)
    {
        *out_data++ = *in_data--;
    }
}
