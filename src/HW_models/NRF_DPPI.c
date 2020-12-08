/*
 * Copyright (c) 2020 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "NRF_DPPI.h"
#include <stdlib.h>
#include "bs_tracing.h"
#include <string.h>

/*
 * DPPI — Distributed programmable peripheral interconnect
 * https://infocenter.nordicsemi.com/index.jsp?topic=%2Fps_nrf52840%2Fkeyfeatures_html5.html&cp=4_0_0
 *
 */

NRF_DPPIC_Type NRF_DPPI_regs; ///< The DPPI registers

/**
 * DPPI module own TASKs handlers
 */
void nrf_dppi_TASK_CHG_ENDIS( int groupnbr, bool enable /*false=disable task*/ )
{
  if ( enable )
  {
    bs_trace_raw_time(9, "dppi: Channel group %i enabled\n", groupnbr);
    NRF_DPPI_regs.CHEN |= NRF_DPPI_regs.CHG[groupnbr];
  } else
  {
    bs_trace_raw_time(9 ,"dppi: Channel group %i disable\n", groupnbr);
    NRF_DPPI_regs.CHEN &= ~NRF_DPPI_regs.CHG[groupnbr];
  }
  //Note of the author: From the spec I cannot guess if these tasks will affect
  //CHEN or a separate hidden register
}

void nrf_dppi_TASK_CHG0_EN() { nrf_dppi_TASK_CHG_ENDIS(0,true); }
void nrf_dppi_TASK_CHG1_EN() { nrf_dppi_TASK_CHG_ENDIS(1,true); }
void nrf_dppi_TASK_CHG2_EN() { nrf_dppi_TASK_CHG_ENDIS(2,true); }
void nrf_dppi_TASK_CHG3_EN() { nrf_dppi_TASK_CHG_ENDIS(3,true); }
void nrf_dppi_TASK_CHG4_EN() { nrf_dppi_TASK_CHG_ENDIS(4,true); }
void nrf_dppi_TASK_CHG5_EN() { nrf_dppi_TASK_CHG_ENDIS(5,true); }
void nrf_dppi_TASK_CHG0_DIS(){ nrf_dppi_TASK_CHG_ENDIS(0,false); }
void nrf_dppi_TASK_CHG1_DIS(){ nrf_dppi_TASK_CHG_ENDIS(1,false); }
void nrf_dppi_TASK_CHG2_DIS(){ nrf_dppi_TASK_CHG_ENDIS(2,false); }
void nrf_dppi_TASK_CHG3_DIS(){ nrf_dppi_TASK_CHG_ENDIS(3,false); }
void nrf_dppi_TASK_CHG4_DIS(){ nrf_dppi_TASK_CHG_ENDIS(4,false); }
void nrf_dppi_TASK_CHG5_DIS(){ nrf_dppi_TASK_CHG_ENDIS(5,false); }

/*Forward declaration*/
struct subscr_list_elem;

//Element of linked list of subscribers
typedef struct subscr_list_elem
{
  task_function_t task_function;
  struct subscr_list_elem * p_next;
} subscr_list_elem_t;

//Table with TASKs subscribing to all channels
static subscr_list_elem_t* dppi_subscriber_list[DPPI_CH_NUM];

//Function for checking if an element is in the subscriber linked list
static bool subscriber_list_check(task_function_t task_function, subscr_list_elem_t * p_head)
{
  if (p_head == NULL)
  {
    return false;
  }

  subscr_list_elem_t *p_last_elem = p_head;
  while (p_last_elem->p_next != NULL)
  {
    if (p_last_elem->task_function == task_function)
    {
      return true;
    }
    p_last_elem = p_last_elem->p_next;
  }

  return false;
}

//Function for appending an element to the subscriber linked list
static void subscriber_list_append(subscr_list_elem_t ** pp_head, task_function_t task_function)
{
  if (!subscriber_list_check(task_function, *pp_head))
  {
    struct subscr_list_elem* p_elem = malloc(sizeof(struct subscr_list_elem));
    if (p_elem == NULL)
    {
      bs_trace_warning_line_time("Memory allocation failed\n");
      return;
    }
    p_elem->task_function = task_function;
    p_elem->p_next = *pp_head;
    *pp_head = p_elem;
  }
}

//Function for removing an element from the subscriber linked list
static void subscriber_list_remove(subscr_list_elem_t ** pp_head, task_function_t task_function)
{
  struct subscr_list_elem* temp = *pp_head;
  subscr_list_elem_t *prev;

  if (temp != NULL && temp->task_function == task_function)
  {
    *pp_head = temp->p_next;
    free(temp);
    return;
  }

  while (temp != NULL && temp->task_function != task_function)
  {
    prev = temp;
    temp = temp->p_next;
  }

  if (temp == NULL)
  {
    return;
  }

  prev->p_next = temp->p_next;
  free(temp);
}

/**
 * Update the subscriber list by adding the task to the specified channel
 */
void nrf_dppi_subscriber_add(uint8_t channel, task_function_t function)
{
  subscriber_list_append(&dppi_subscriber_list[channel], function);
}

/**
 * Update the subscriber list by removing the task from all channels
 */
void nrf_dppi_subscriber_remove(task_function_t function)
{
  for(int i=0; i < DPPI_CH_NUM; i++)
  {
    subscriber_list_remove(&dppi_subscriber_list[i], function);
  }
}

/**
 * Publish to a channel by triggering all tasks subscribing to the channel
 */
void nrf_dppi_publish(uint8_t channel)
{
  struct subscr_list_elem* temp = dppi_subscriber_list[channel];

  while (temp != NULL)
  {
    temp->task_function();
    temp = temp->p_next;
  }
}


/**
 * Initialize the DPPI model
 */
void nrf_dppi_init(void)
{
  memset(&NRF_DPPI_regs, 0, sizeof(NRF_DPPI_regs));
  memset(dppi_subscriber_list, 0, sizeof(dppi_subscriber_list));
}

/**
 * Cleanup the DPPI model before exiting the program
 */
void nrf_dppi_clean_up()
{
  for(int i=0; i < DPPI_CH_NUM; i++)
  {
    struct subscr_list_elem* current = dppi_subscriber_list[i];
    struct subscr_list_elem* next;

    while (current != NULL)
    {
      next = current->p_next;
      free(current);
      current = next;
    }
    dppi_subscriber_list[i] = NULL;
  }
}


/**
 * Update DPPI CHEN mask after a write to CHENSET
 * (writes to CHEN do not need sideeffects)
 */
void nrf_dppi_regw_sideeffects_CHENSET()
{
  if ( NRF_DPPI_regs.CHENSET != 0 )
  {
    NRF_DPPI_regs.CHEN |= NRF_DPPI_regs.CHENSET;
    NRF_DPPI_regs.CHENSET = 0;
  }
}

/**
 * Update DPPI CHEN mask after a write to CHENCLR
 * (writes to CHEN do not need sideeffects)
 */
void nrf_dppi_regw_sideeffects_CHENCLR()
{
  if ( NRF_DPPI_regs.CHENCLR != 0 )
  {
    NRF_DPPI_regs.CHEN &= ~NRF_DPPI_regs.CHENCLR;
    NRF_DPPI_regs.CHENCLR = 0;
  }
}
