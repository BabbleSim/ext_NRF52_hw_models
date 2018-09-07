/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef NRF_HW_DUMP_TOP_H
#define NRF_HW_DUMP_TOP_H

/**
 * List of files identifiers this HW models will dump:
 *   This identifiers should be used when referring to the files
 */
#define NRF_HW_DUMP_FILES_TYPES \
  NRF_dummy
/*be careful to not have a comma in the last entry*/


/**
 * For each file identifier in NRF_HW_DUMP_FILES_TABLE, an entry in the following table should be created
 * The position in the table gives which identifier it refers to (so keep the order)
 */
#define NRF_HW_DUMP_FILES_TABLE \
  /*   File name (suffix)   , <dump_level> , <has it been activated> , <file ptr>,  <function to dump the header>,  */  \
  { "NRF_dummy"             ,     6                  , 0                , NULL   ,   NULL         },\
  /*NRF_dummy just to have something while we dont have anything else (the table cannot be empty)*/


/**
 * Prototypes of the header dumping functions:
 */

#endif
