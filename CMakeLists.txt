# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2019 Oticon A/S

# CMake file to compile this BabbleSim component as a west module in Zephyr

if(CONFIG_SOC_SERIES_BSIM_NRFXX)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nrfx/hal_replacements
  )

  zephyr_include_directories(
    src/HW_models/
    src/nrfx/nrfx_replacements
  )

  if(CONFIG_SOC_SERIES_BSIM_NRF52X)
    file(STRINGS make_inc/52833_hw_files HW_MODEL_SRCS)
    file(STRINGS make_inc/52833_hal_files HAL_SRCS)
    set(VARIANT_FLAGS 
        -DNRF52833_XXAA
        -DNRF52833
    )
  elseif(CONFIG_SOC_SERIES_BSIM_NRF53X)
    file(STRINGS make_inc/5340_hw_files HW_MODEL_SRCS)

    if(CONFIG_BOARD_NRF5340BSIM_NRF5340_CPUNET)
      file(STRINGS make_inc/5340_net_hal_files HAL_SRCS)
      set(VARIANT_FLAGS
        -DNRF5340_XXAA_NETWORK
        -DNRF5340
      )
    elseif(CONFIG_BOARD_NRF5340BSIM_NRF5340_CPUAPP)
      file(STRINGS make_inc/5340_app_hal_files HAL_SRCS)
      set(VARIANT_FLAGS
        -DNRF5340_XXAA_APPLICATION
        -DNRF5340
      )
    endif()
  elseif(CONFIG_SOC_SERIES_BSIM_NRF54LX)
    file(STRINGS make_inc/54L15_hw_files HW_MODEL_SRCS)
    file(STRINGS make_inc/54L15_app_hal_files HAL_SRCS)
    set(VARIANT_FLAGS
        -DNRF54L15_XXAA
        -DNRF_APPLICATION
        -DNRF54L15
    )
  endif()

  # The nrfx HAL is built in the context of the embedded software
  zephyr_library()
  zephyr_library_sources(${HAL_SRCS})

  zephyr_library_include_directories(
    $ENV{BSIM_COMPONENTS_PATH}/libUtilv1/src/
    src/HW_models/
  )

  # The HW models are built in the runner context, so they have access to the host OS
  target_sources(native_simulator INTERFACE ${HW_MODEL_SRCS})

  target_compile_options(native_simulator BEFORE INTERFACE 
    -I${NSI_DIR}/common/src/include
    -I${NSI_DIR}/common/src/
    -I$ENV{BSIM_COMPONENTS_PATH}/libUtilv1/src/
    -I$ENV{BSIM_COMPONENTS_PATH}/libPhyComv1/src/
    -I$ENV{BSIM_COMPONENTS_PATH}/ext_2G4_libPhyComv1/src/
    -I$ENV{BSIM_COMPONENTS_PATH}/libRandv2/src/
    -I${CMAKE_CURRENT_SOURCE_DIR}/src/HW_models/
    -I${CMAKE_CURRENT_SOURCE_DIR}/src/nrfx_config
    -I${CMAKE_CURRENT_SOURCE_DIR}/src/nrfx/mdk_replacements
    -I${CMAKE_CURRENT_SOURCE_DIR}/src/nrfx/nrfx_replacements
    -I${NRFX_DIR}
    -I${NRFX_DIR}/mdk
    ${VARIANT_FLAGS}
    -std=gnu11 #The nrfx hal uses non standard features which will cause warnings otherwise
  )

endif()
