/*
 * Copyright (c) 2017 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bs_types.h"
#include "bs_tracing.h"
#include "bs_pc_2G4.h"
#include "NRF_HWLowL.h"
#include "xo_if.h"

/*
 * Do not actually connect to the phy.
 * This is a minor debug switch to allow running the device without needing
 * to run the phy. It can be useful if for example the device crashes
 * during boot, or if running a test without any radio activity.
 * Note that as soon as the radio attempts to have any activity the
 * device will crash with a "not connected" error
 */
static bool nosim;

void hwll_set_nosim(bool new_nosim){
  nosim = new_nosim;
}

/**
 * Return the equivalent phy time from a device time
 */
bs_time_t hwll_phy_time_from_dev(bs_time_t d_t){
  bs_time_t phy_t;
  if ( d_t != TIME_NEVER ) {
    phy_t = (bs_time_t)(1e6*phy_time_from_dev(((long double)d_t)*1e-6 ) + 0.5);
    //Note: in x86 compiling with GCC for linux long double is 80bit (64bits
    // mantissa), and therefore precission should be kept
  } else {
    phy_t = TIME_NEVER;
  }
  return phy_t;
}

/**
 * Return the equivalent device time from a phy time
 */
uint64_t hwll_dev_time_from_phy(bs_time_t p_t){
  bs_time_t dev_t;
  if ( p_t != TIME_NEVER ) {
    dev_t = (bs_time_t)(1e6*dev_time_from_phy(((long double)p_t)*1e-6 ) + 0.5);
    //Note: in x86 compiling with GCC for linux long double is 80bit (64bits
    // mantissa), and therefore precission should be kept
  } else {
    dev_t = p_t;
  }
  return dev_t;
}

/**
 * Every time we would like to "inform" the phy about our timing
 * (if we are advancing it a lot relative to the phy time while idling in the radio)
 * we can use this function to cause a wait
 */
void hwll_sync_time_with_phy(bs_time_t d_time) {
  if (nosim)
    return;

  pb_wait_t wait;

  if ( d_time != TIME_NEVER ){
    wait.end = hwll_phy_time_from_dev(d_time - 2);
    //Wait for 2us less than the actual time, to allow for error with the
    //rounding of the clock drift + 1 us extra in case the next Tx or Rx
    //happens just in this same Time
  } else {
    wait.end = TIME_NEVER;
  }

  if ( p2G4_dev_req_wait_nc_b(&wait) != 0){
    bs_trace_raw_manual_time(3, d_time, "The phy disconnected us\n");
    hwll_disconnect_phy_and_exit();
  }
}

/**
 * Wait until the Phy reaches a given *Phy* simulated time
 *
 * Note: This function should only be called in very special cases.
 * Actual HW models are not expected to use it, instead they should rely
 * on the time machine auto-synchronization mechanism.
 */
void hwll_wait_for_phy_simu_time(bs_time_t phy_time){
  if (nosim)
    return;

  pb_wait_t wait;

  wait.end = phy_time;

  if ( p2G4_dev_req_wait_nc_b(&wait) != 0 ){
    bs_trace_raw_manual_time(3, phy_time, "The phy disconnected us\n");
    hwll_disconnect_phy_and_exit();
  }
}

/**
 * Connect to the phy
 */
int hwll_connect_to_phy(uint d, const char* s, const char* p){
  if (!nosim) {
    return p2G4_dev_initcom_nc(d, s, p);
  } else {
    return 0;
  }
}

/**
 * Disconnect from the phy, and ask it to end the simulation
 */
void hwll_terminate_simulation(void) {
  if (!nosim) {
    p2G4_dev_terminate_nc();
  }
}

/**
 * Disconnect from the phy, but let the simulation continue without us
 */
void hwll_disconnect_phy(void) {
  if (!nosim) {
    p2G4_dev_disconnect_nc();
  }
}

/**
 * Terminate the simulation for this device:
 *  Disconnection from the phy and exit
 */
void hwll_disconnect_phy_and_exit(void) {
  hwll_disconnect_phy();
  bs_trace_exit_line("\n");
}
