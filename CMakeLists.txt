# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2019 Oticon A/S

# CMake file to compile this BabbleSim component as a west module in Zephyr

if(CONFIG_BOARD_NRF52_BSIM)

  if (NOT DEFINED ENV{BSIM_COMPONENTS_PATH})
    message(FATAL_ERROR "This Zephyr module requires the BabbleSim simulator.\
 Please set the  environment variable BSIM_COMPONENTS_PATH to point to its\
 components folder. More information can be found in\
 https://babblesim.github.io/folder_structure_and_env.html")
  endif()
  if (NOT DEFINED ENV{BSIM_OUT_PATH})
    message(FATAL_ERROR "This Zephyr module requires the BabbleSim simulator.\
 Please set the  environment variable BSIM_OUT_PATH to point to the folder\
 where the simulator is compiled to. More information can be found in\
 https://babblesim.github.io/folder_structure_and_env.html")
  endif()

  # These include directories must be added before those from the hal_nordic
  # module, so that the local modified versions of nrfx files are used instead
  # of those from the original nrfx.
  target_include_directories(zephyr_interface BEFORE INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nrfx/mdk_replacements
  )

  zephyr_include_directories(
    src/HW_models/
    src/nrfx/nrfx_replacements
  )

  zephyr_library()
  zephyr_library_compile_definitions(NO_POSIX_CHEATS)

  file(GLOB_RECURSE HW_MODEL_SRCS . src/*.c)
  zephyr_library_sources(${HW_MODEL_SRCS})

  zephyr_library_include_directories(
    $ENV{BSIM_COMPONENTS_PATH}/libUtilv1/src/
    $ENV{BSIM_COMPONENTS_PATH}/libPhyComv1/src/
    $ENV{BSIM_COMPONENTS_PATH}/ext_2G4_libPhyComv1/src/
    $ENV{BSIM_COMPONENTS_PATH}/libRandv2/src/
    src/HW_models/
  )

endif()
