/******** DO NOT EDIT THIS FILE ********/
/* 
 * test_sem_ipc_jobqueue.h - structures and function declarations for unit tests
 * of sem_ipc_jobqueue functions.
 * 
 */  
#ifndef _TEST_JOB_QUEUE_H
#define _TEST_JOB_QUEUE_H
#define MUNIT_ENABLE_ASSERT_ALIASES
#include "munit/munit.h"

MunitResult test_sem_jobqueue_capacity(const MunitParameter params[], 
    void* fixture);
MunitResult test_sem_jobqueue_capacity_null(const MunitParameter params[], 
    void* fixture);

MunitResult test_sem_jobqueue_dequeue_null(const MunitParameter params[], 
    void* fixture);

MunitResult test_sem_jobqueue_enqueue_null(const MunitParameter params[], 
    void* fixture);

MunitResult test_sem_jobqueue_is_empty_null(const MunitParameter params[], 
    void* fixture);

MunitResult test_sem_jobqueue_is_full_null(const MunitParameter params[], 
    void* fixture);

MunitResult test_sem_jobqueue_peekhead_null(const MunitParameter params[], 
    void* fixture);

MunitResult test_sem_jobqueue_peektail_null(const MunitParameter params[], 
    void* fixture);

/* two-process tests */
MunitResult test_sem_jobqueue_2proc_enqueue_dequeue(
    const MunitParameter params[], void* fixture);
MunitResult test_sem_jobqueue_2proc_peekhead(const MunitParameter params[], 
    void* fixture);
MunitResult test_sem_jobqueue_2proc_peektail(const MunitParameter params[], 
    void* fixture);

void* test_setup(const MunitParameter params[], void* user_data);
void test_tear_down(void* fixture);

static MunitTest tests[] = {
    { "/test_sem_jobqueue_capacity", test_sem_jobqueue_capacity, test_setup,
        test_tear_down, MUNIT_TEST_OPTION_NONE, NULL },
    { "/test_sem_jobqueue_capacity_null", test_sem_jobqueue_capacity_null,
        test_setup, test_tear_down, MUNIT_TEST_OPTION_NONE, NULL },
        
    { "/test_sem_jobqueue_dequeue_null", test_sem_jobqueue_dequeue_null,
        test_setup, test_tear_down, MUNIT_TEST_OPTION_NONE, NULL },
        
    { "/test_sem_jobqueue_enqueue_null", test_sem_jobqueue_enqueue_null,
        test_setup, test_tear_down, MUNIT_TEST_OPTION_NONE, NULL },

    { "/test_sem_jobqueue_is_empty_null",
        test_sem_jobqueue_is_empty_null, test_setup, test_tear_down,
        MUNIT_TEST_OPTION_NONE, NULL },

    { "/test_sem_jobqueue_is_full_null", test_sem_jobqueue_is_full_null,
        test_setup, test_tear_down, MUNIT_TEST_OPTION_NONE, NULL },

    { "/test_sem_jobqueue_peekhead_null", test_sem_jobqueue_peekhead_null,
        test_setup, test_tear_down, MUNIT_TEST_OPTION_NONE, NULL },

    { "/test_sem_jobqueue_peektail_null", test_sem_jobqueue_peektail_null,
        test_setup, test_tear_down, MUNIT_TEST_OPTION_NONE, NULL },

    /* two-process tests */
    { "/test_sem_jobqueue_2proc_enqueue_dequeue", 
        test_sem_jobqueue_2proc_enqueue_dequeue, NULL, NULL, 
        MUNIT_TEST_OPTION_NONE,  NULL },
    { "/test_sem_jobqueue_2proc_peekhead", test_sem_jobqueue_2proc_peekhead,
        NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/test_sem_jobqueue_2proc_peektail", test_sem_jobqueue_2proc_peektail,
        NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

static const MunitSuite suite = {
    "/test_sem_jobqueue", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE 
};    


#endif