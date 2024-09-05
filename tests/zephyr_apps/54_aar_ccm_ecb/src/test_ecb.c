/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdint.h>
#include <nrf.h>
#include <hal/nrf_ecb.h>
#include <zephyr/kernel.h>
#include <zephyr/ztest.h>
#include "common.h"

#define ECB NRF_ECB00
#define ECB_JOB_ATTRIB (0xB)

static void start_ecb_and_wait(void) {
	nrf_ecb_event_clear(ECB, NRF_ECB_EVENT_END);
	nrf_ecb_event_clear(ECB, NRF_ECB_EVENT_ERROR);
	nrf_ecb_task_trigger(ECB, NRF_ECB_TASK_START);

	while (!nrf_ecb_event_check(ECB, NRF_ECB_EVENT_END) && !nrf_ecb_event_check(ECB, NRF_ECB_EVENT_ERROR))
	{
		k_busy_wait(1);
	}
}

static inline void set_ecb_key_be(NRF_ECB_Type *p_reg, uint32_t const * p_key) {
  uint8_t key_swapped[16];
  for (int i = 0; i < 16; i++) {
    key_swapped[15-i] = ((uint8_t*)p_key)[i];
  }
  nrf_ecb_key_set(p_reg, (const uint32_t*)key_swapped);
}

ZTEST(nrf_ecb_tests, test_ecb_1)
{
	//ztest_test_skip();
	// Key, Clear text, ciphertext data sample 1. Ref http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
	uint8_t KEY_SAMPLE_1[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
				  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
	uint8_t CLRTXT_SAMPLE_1[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
				     0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
	// We will split this in a different way from the other jobs to test on sequential input data
	uint32_t job1_len_1 = 10;
	uint32_t job1_len_2 = 1;
	uint32_t job1_len_3 = 5;
	uint8_t *CLRTXT_SAMPLE_1_1 = &CLRTXT_SAMPLE_1[0];
	uint8_t *CLRTXT_SAMPLE_1_2 = &CLRTXT_SAMPLE_1[job1_len_1];
	uint8_t *CLRTXT_SAMPLE_1_3 = &CLRTXT_SAMPLE_1[job1_len_1 + job1_len_2];
	uint8_t CIPTXT_SAMPLE_1[] = {0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
				     0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a};

	// Key, Clear text, ciphertext data sample 2. Ref http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
	uint8_t KEY_SAMPLE_2[] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t CLRTXT_SAMPLE_2_1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t CLRTXT_SAMPLE_2_2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t CIPTXT_SAMPLE_2[] = {0x0e, 0xdd, 0x33, 0xd3, 0xc6, 0x21, 0xe5, 0x46,
				     0x45, 0x5b, 0xd8, 0xba, 0x14, 0x18, 0xbe, 0xc8};

	// Key, Clear text, ciphertext data sample 3. Ref http://www.hanewin.net/encrypt/aes/aes-test.htm
	uint8_t KEY_SAMPLE_3[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
				  0x08, 0x09, 0x0a, 0x0b, 0x00, 0x00, 0x00, 0x00};
	uint8_t CLRTXT_SAMPLE_3[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
				     0x88, 0x99, 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t CIPTXT_SAMPLE_3[] = {0xba, 0xb7, 0xb2, 0x59, 0xcc, 0x75, 0x3c, 0x90,
				     0xdc, 0x4e, 0xf6, 0xdc, 0xa7, 0xd9, 0x30, 0x2d};

	uint32_t job2_len_1 = sizeof(CLRTXT_SAMPLE_2_1);
	uint32_t job2_len_2 = sizeof(CLRTXT_SAMPLE_2_2);
	uint32_t job3_len = sizeof(CLRTXT_SAMPLE_3);

	uint8_t outjob1_data_1[10] = {0};
	uint8_t outjob1_data_2[6] = {0};
	uint8_t outjob2_data[16] = {0};

	uint8_t buf[16] = {0}; // Different setup to check for accidental memory overwrites
	uint32_t outjob3_len_1 = 5;
	uint32_t outjob3_len_2 = 10;
	uint32_t outjob3_len_3 = 1;
	uint8_t *outjob3_data_1 = &buf[0];
	uint8_t *outjob3_data_2 = &buf[outjob3_len_1];
	uint8_t *outjob3_data_3 = &buf[outjob3_len_1 + outjob3_len_2];

	uint32_t outjob1_len_1 = sizeof(outjob1_data_1);
	uint32_t outjob1_len_2 = sizeof(outjob1_data_2);
	uint32_t outjob2_len = sizeof(outjob2_data);

	// Setup job lists
	job_t encrypt_injob1[] = {
			{(uint8_t *)&CLRTXT_SAMPLE_1_1[0], job1_len_1, ECB_JOB_ATTRIB},
			{(uint8_t *)&CLRTXT_SAMPLE_1_2[0], job1_len_2, ECB_JOB_ATTRIB},
			{(uint8_t *)&CLRTXT_SAMPLE_1_3[0], job1_len_3, ECB_JOB_ATTRIB},
			{0x0, 0x0}}; // Marking end of job list
	job_t encrypt_injob2[] = {
			{(uint8_t *)&CLRTXT_SAMPLE_2_1[0], job2_len_1, ECB_JOB_ATTRIB},
			{(uint8_t *)&CLRTXT_SAMPLE_2_2[0], job2_len_2, ECB_JOB_ATTRIB},
			{0x0, 0x0}};
	job_t encrypt_injob3[] = {
			{(uint8_t *)&CLRTXT_SAMPLE_3[0], job3_len, ECB_JOB_ATTRIB},
			{0x0, 0x0}};
	job_t encrypt_outjob1[] = {
			{(uint8_t *)&outjob1_data_1[0], outjob1_len_1, ECB_JOB_ATTRIB},
			{(uint8_t *)&outjob1_data_2[0], outjob1_len_2, ECB_JOB_ATTRIB},
			{0x0, 0x0}};
	job_t encrypt_outjob2[] = {
			{(uint8_t *)&outjob2_data[0], outjob2_len, ECB_JOB_ATTRIB},
			{0x0, 0x0}};
	job_t encrypt_outjob3[] = {
			{(uint8_t *)&outjob3_data_1[0], outjob3_len_1, ECB_JOB_ATTRIB},
			{(uint8_t *)&outjob3_data_2[0], outjob3_len_2, ECB_JOB_ATTRIB},
			{(uint8_t *)&outjob3_data_3[0], outjob3_len_3, ECB_JOB_ATTRIB},
			{0x0, 0x0}};

	printf("Starting first job\n");

	set_ecb_key_be(ECB, (uint32_t *)KEY_SAMPLE_1);
	nrf_ecb_in_ptr_set(ECB, (nrf_vdma_job_t const *)encrypt_injob1);
	nrf_ecb_out_ptr_set(ECB, (nrf_vdma_job_t const *)encrypt_outjob1);

	start_ecb_and_wait();

	zassert_false(ECB->EVENTS_ERROR, "Error in ECB encryption process!\n");

	printf("Starting second job\n");

	set_ecb_key_be(ECB, (uint32_t *)KEY_SAMPLE_2);
	nrf_ecb_in_ptr_set(ECB, (nrf_vdma_job_t const *)encrypt_injob2);
	nrf_ecb_out_ptr_set(ECB, (nrf_vdma_job_t const *)encrypt_outjob2);

	start_ecb_and_wait();

	zassert_false(ECB->EVENTS_ERROR, "Error in ECB encryption process!\n");

	printf("Starting third job\n");

	set_ecb_key_be(ECB, (uint32_t *)KEY_SAMPLE_3);

	nrf_ecb_in_ptr_set(ECB, (nrf_vdma_job_t const *)encrypt_injob3);
	nrf_ecb_out_ptr_set(ECB, (nrf_vdma_job_t const *)encrypt_outjob3);

	start_ecb_and_wait();

	zassert_false(ECB->EVENTS_ERROR, "Error in ECB encryption process!\n");

	int error = 0;
	printf("Verifying that encryption is correct...\n");
	/* Checking job 1 */
	printf("\nLength of scattered outputs in job 1: %u %u\n", encrypt_outjob1[0].length,
			encrypt_outjob1[1].length);
	printf("Ecb encryption of first job:  ");
	for (int i = 0; i < encrypt_outjob1[0].length; i++) {
		printf("%02x", encrypt_outjob1[0].job_ptr[i]);
	}
	for (int i = 0; i < encrypt_outjob1[1].length; i++) {
		printf("%02x", encrypt_outjob1[1].job_ptr[i]);
	}
	printf("\nTrue encryption of first job: ");
	for (int i = 0; i < sizeof(CIPTXT_SAMPLE_1); i++) {
		printf("%02x", CIPTXT_SAMPLE_1[i]);
	}
	for (int i = 0; i < encrypt_outjob1[0].length; i++) {
		if (encrypt_outjob1[0].job_ptr[i] != CIPTXT_SAMPLE_1[i]) {
			printf("\nERROR in encryption of first job!\n");
			error = 1;
			break;
		}
	}
	for (int i = 0; i < encrypt_outjob1[1].length; i++) {
		if (encrypt_outjob1[1].job_ptr[i] != CIPTXT_SAMPLE_1[i + encrypt_outjob1[0].length]) {
			printf("\nERROR in encryption of first job!\n");
			error = 1;
			break;
		}
	}

	/* Checking job 2 */
	printf("\n\nLength of single output in job 2: %u", encrypt_outjob2[0].length);
	printf("\nEcb encryption of second job:  ");
	for (int i = 0; i < encrypt_outjob2[0].length; i++) {
		printf("%02x", encrypt_outjob2[0].job_ptr[i]);
	}
	printf("\nTrue encryption of second job: ");
	for (int i = 0; i < sizeof(CIPTXT_SAMPLE_1); i++) {
		printf("%02x", CIPTXT_SAMPLE_2[i]);
	}
	for (int i = 0; i < encrypt_outjob2[0].length; i++) {
		if (encrypt_outjob2[0].job_ptr[i] != CIPTXT_SAMPLE_2[i]) {
			printf("\nERROR in encryption of second job!\n");
			error = 1;
			break;
		}
	}
	/* Checking job 3 */
	printf("\n\nLength of scattered outputs in job 3: %u %u %u", encrypt_outjob3[0].length,
			encrypt_outjob3[1].length, encrypt_outjob3[2].length);
	printf("\nEcb encryption of third job:  ");
	for (int i = 0; i < encrypt_outjob3[0].length; i++) {
		printf("%02x", encrypt_outjob3[0].job_ptr[i]);
	}
	for (int i = 0; i < encrypt_outjob3[1].length; i++) {
		printf("%02x", encrypt_outjob3[1].job_ptr[i]);
	}
	for (int i = 0; i < encrypt_outjob3[2].length; i++) {
		printf("%02x", encrypt_outjob3[2].job_ptr[i]);
	}
	printf("\nTrue encryption of third job: ");
	for (int i = 0; i < sizeof(CIPTXT_SAMPLE_3); i++) {
		printf("%02x", CIPTXT_SAMPLE_3[i]);
	}
	for (int i = 0; i < encrypt_outjob3[0].length; i++) {
		if (encrypt_outjob3[0].job_ptr[i] != CIPTXT_SAMPLE_3[i]) {
			printf("\nERROR in encryption of third job!\n");
			error = 1;
			break;
		}
	}
	for (int i = 0; i < encrypt_outjob3[1].length; i++) {
		if (encrypt_outjob3[1].job_ptr[i] != CIPTXT_SAMPLE_3[i + encrypt_outjob3[0].length]) {
			printf("\nERROR in encryption of third job!\n");
			error = 1;
			break;
		}
	}
	for (int i = 0; i < encrypt_outjob3[2].length; i++) {
		if (encrypt_outjob3[2].job_ptr[i] !=
				CIPTXT_SAMPLE_3[i + encrypt_outjob3[0].length + encrypt_outjob3[1].length]) {
			printf("\nERROR in encryption of third job!\n");
			error = 1;
			break;
		}
	}

	zassert_false(error, "Error in ECB encryption process!\n");
	printf("\nEncryption successful!\n");

	printf("\nStarting fourth job with faulty input\n"); //TODO: clarify, this is not an error
	uint32_t len_error = sizeof(CLRTXT_SAMPLE_1_1);
	job_t encrypt_in_jobs_error[] = {{(uint8_t *)&CLRTXT_SAMPLE_1_1[0], len_error, ECB_JOB_ATTRIB}, {0x0, 0x0, 0x0}};
	nrf_ecb_in_ptr_set(ECB, (nrf_vdma_job_t const *)encrypt_in_jobs_error);

	start_ecb_and_wait();

	//zassert_true(ECB->EVENTS_ERROR, "Did not successfully trigger error in ECB encryption!\n"); //TODO: clarify, this is not an error
}

ZTEST_SUITE(nrf_ecb_tests, NULL, NULL, NULL, NULL, NULL);
