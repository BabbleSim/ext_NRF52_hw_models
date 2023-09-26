/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NRF_HW_MODEL_NHW_NVM_BACKEND_H
#define _NRF_HW_MODEL_NHW_NVM_BACKEND_H

#include <stdint.h>
#include "bs_types.h"
#include "nsi_utils.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
  uint8_t *storage;
  const char *file_path;
  const char *type_s;
  int fd;
  size_t size;
  bool erase_at_start;
  bool rm_at_exit;
  bool in_ram;
} nvm_storage_state_t;

struct nhw_nvm_st_args_t {
  char *file;
  bool erase;
  bool rm;
  bool in_ram;
};

void nhw_nvm_initialize_data_storage(nvm_storage_state_t *st);
void nhw_nvm_clear_storage(nvm_storage_state_t *st);
void nhw_nvm_init_storage(nvm_storage_state_t *st, struct nhw_nvm_st_args_t *args,
                          size_t size, char *type);

#define NVM_BACKEND_PARAMS(x, arg, X)                                         \
{ .is_switch = true,                                                          \
  .option = NSI_STRINGIFY(x) "_erase",                                        \
  .type = 'b',                                                                \
  .dest = (void*)&nvmc_args. arg .erase,                                        \
  .descript = "Reset the " # X " storage to their erase values (0xFF) at boot"\
},                                                                            \
{ .option = NSI_STRINGIFY(x) "_file",                                         \
  .name = "path",                                                             \
  .type = 's',                                                                \
  .dest = (void*)&nvmc_args. arg .file,                                         \
  .call_when_found = arg_##x##_file_found,                                    \
  .descript = "Path to the binary file where the " #X " content "             \
  "is stored (if set, toggles " NSI_STRINGIFY(x) "_in_ram to false)"          \
},                                                                            \
{ .option = NSI_STRINGIFY(x),                                                 \
  .name = "path",                                                             \
  .type = 's',                                                                \
  .dest = (void*)&nvmc_args. arg . file,                                        \
  .call_when_found = arg_##x##_file_found,                                    \
  .descript = "Alias for " NSI_STRINGIFY(x) "_file"                           \
},                                                                            \
{ .is_switch = true,                                                          \
  .option = NSI_STRINGIFY(x) "_rm",                                           \
  .type = 'b',                                                                \
  .dest = (void*)&nvmc_args. arg .rm,                                           \
  .descript = "Remove the " # X " file when terminating the execution "       \
               "(default no)"                                                 \
},                                                                            \
{ .is_switch = true,                                                          \
  .option = NSI_STRINGIFY(x) "_in_ram",                                       \
  .type = 'b',                                                                \
  .call_when_found = arg_##x##_in_ram_found,                                  \
  .descript = "(default)  Instead of a file, keep the " #X " content in RAM." \
        "If this is set " NSI_STRINGIFY(x) "_erase/_file & _rm "              \
        "are ignored, and the " #X " content is always reset at startup"      \
}

#define NVM_BACKEND_PARAMS_CALLBACS(x, args) \
static void arg_##x##_file_found(char *argv, int offset){ \
  nvmc_args. args .in_ram = false; \
} \
static void arg_##x##_in_ram_found(char *argv, int offset){ \
  nvmc_args. args .in_ram = true; \
}


#ifdef __cplusplus
}
#endif

#endif /* _NRF_HW_MODEL_NHW_NVM_BACKEND_H */
