/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_NHW_NVM_COMMON_H
#define _NRF_HW_MODEL_NHW_NVM_COMMON_H

#ifdef __cplusplus
extern "C"{
#endif

enum flash_op_t {flash_idle = 0, flash_write, flash_erase, flash_erase_partial, flash_erase_uicr, flash_erase_all};

#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_NHW_NVM_COMMON_H */
