/******** DO NOT EDIT THIS FILE ********/
#include <stdio.h>
#include "test_job.h"
#include "../job.h"

#define TEST_LABELS 16
static char* label_in[TEST_LABELS] = {
    "",
    "a",
    "ab",
    "abc",
    "abcd",
    "abcdefghijklmnopqrstuvwxyz",
    "abcdefghijklmnopqrstuvwxyz0",
    "abcdefghijklmnopqrstuvwxyz01",
    "abcdefghijklmnopqrstuvwxyz012",
    "abcdefghijklmnopqrstuvwxyz0123",
    "abcdefghijklmnopqrstuvwxyz01234",
    "abcdefghijklmnopqrstuvwxyz012345",
    "abcdefghijklmnopqrstuvwxyz0123456",
    "abcdefghijklmnopqrstuvwxyz01234567",
    "abcdefghijklmnopqrstuvwxyz012345678",
    "abcdefghijklmnopqrstuvwxyz0123456789",
};

static int label_in_len[TEST_LABELS] = {
    0, 1, 2, 3, 4, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36
};

static char* expected_label[TEST_LABELS] = {
    "*******************************",
    "a******************************",
    "ab*****************************",
    "abc****************************",
    "abcd***************************",
    "abcdefghijklmnopqrstuvwxyz*****",
    "abcdefghijklmnopqrstuvwxyz0****",
    "abcdefghijklmnopqrstuvwxyz01***",
    "abcdefghijklmnopqrstuvwxyz012**",
    "abcdefghijklmnopqrstuvwxyz0123*",
    "abcdefghijklmnopqrstuvwxyz01234",
    "abcdefghijklmnopqrstuvwxyz01234",
    "abcdefghijklmnopqrstuvwxyz01234",
    "abcdefghijklmnopqrstuvwxyz01234",
    "abcdefghijklmnopqrstuvwxyz01234",
    "abcdefghijklmnopqrstuvwxyz01234",
};

int main(int argc, char** argv) {
    return munit_suite_main(&suite, NULL, argc, argv);
}

void set_test_job(job_t* job, pid_t pid, unsigned int i, int label_idx) {
    job->pid = pid;
    job->id = i;
    
    if (label_idx < 0) {
        memset(job->label, 0, MAX_NAME_SIZE);
    } else {
        strncpy(job->label, expected_label[label_idx], MAX_NAME_SIZE - 1);
        job->label[MAX_NAME_SIZE - 1] = '\0';
    }
}

bool equal_jobs(job_t* j1, job_t* j2) {
    return j1->pid == j2->pid && j1->id == j2->id 
            && !strncmp(j1->label, j2->label, MAX_NAME_SIZE);
}

void assert_init_job(job_t* job) {
    assert_not_null(job);
    assert_int(job->pid, ==, 0);
    assert_uint(job->id, ==, 0);
    
    for (int i = 0; i < MAX_NAME_SIZE; i++)
        assert_char(job->label[i], ==, '\0');
}

void assert_job_equalities(job_t* j1, job_t* j2) {
    assert_true(job_is_equal(j1, j1));
    assert_true(job_is_equal(j2, j2));
    assert_true(job_is_equal(j1, j2));
    
    int n = munit_rand_int_range(1, 100);
    j1->pid = j1->pid + n;    
    assert_false(job_is_equal(j1, j2));    
    j1->pid = j1->pid - n;    
    assert_true(job_is_equal(j1, j2));
    
    n = munit_rand_int_range(1, 100);
    j1->id = j1->id + n;    
    assert_false(job_is_equal(j1, j2));    
    j1->id = j1->id - n;    
    assert_true(job_is_equal(j1, j2));
    
    n = munit_rand_int_range(1, MAX_NAME_SIZE - 1);
    j1->label[n-1] = j1->label[n-1] - n;
    assert_false(job_is_equal(j1, j2));
    j1->label[n-1] = j1->label[n-1] + n;
    assert_true(job_is_equal(j1, j2));
}

void assert_valid_job(job_t* job, pid_t expected_pid, unsigned int expected_id, 
    int label_idx) {
    assert_not_null(job);
    assert_int(job->pid, ==, expected_pid);
    assert_uint(job->id, ==, expected_id);
    
    assert_string_equal(job->label, expected_label[label_idx]);
    assert_int(strnlen(expected_label[label_idx], MAX_NAME_SIZE + 1), ==, 
        MAX_NAME_SIZE - 1);
    assert_int(strnlen(job->label, MAX_NAME_SIZE + 1), ==, MAX_NAME_SIZE - 1);
    assert_char(job->label[MAX_NAME_SIZE - 1], ==, '\0'); 
}

MunitResult test_job_new(const MunitParameter params[], void* fixture) {
    pid_t pid = getpid();
    
    for (int i = 0; i < TEST_LABELS; i++) {
        munit_logf(MUNIT_LOG_DEBUG, "test label %d: %s", i, label_in[i]);
        assert_int(strnlen(label_in[i], MAX_NAME_SIZE * 2), 
            ==, label_in_len[i]);
        job_t* job = job_new(pid, i, label_in[i]);
        
        munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_ITER_FMT, 
            __LINE__ + 1,  "assert_valid_job", i);
        assert_valid_job(job, pid, i, i);
        
        free(job);
    }

    return MUNIT_OK;
}

MunitResult test_job_new_null(const MunitParameter params[], 
    void* fixture) {
    pid_t pid = getpid();
    job_t* job = job_new(pid, 1, NULL);
    
    munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_FMT, 
            __LINE__ + 1,  "assert_valid_job");
    assert_valid_job(job, pid, 1, 0);

    free(job);
    
    return MUNIT_OK;
}

MunitResult test_job_copy_stack(const MunitParameter params[], void* fixture) {
    pid_t pid = getpid();
    job_t src;
    job_t dst;
    job_t pre_cpy_dst = {-1, 0, "*"};
    
    for (int i = 0; i < TEST_LABELS; i++) {
        dst = pre_cpy_dst;
        set_test_job(&src, pid, i, i);
        job_t* cpy = job_copy(&dst, &src);
        assert_ptr_equal(cpy, &dst);
        assert_ptr_equal(cpy->label, dst.label);
        assert_ptr_not_equal(cpy, &src);
        assert_ptr_not_equal(cpy->label, src.label);
        assert_false(equal_jobs(&src, &pre_cpy_dst));
        assert_true(equal_jobs(cpy, &src));
        assert_false(equal_jobs(cpy, &pre_cpy_dst));
        
        munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_ITER_FMT, 
            __LINE__ + 1,  "assert_valid_job", i);
        assert_valid_job(cpy, pid, i, i);
    }
    
    return MUNIT_OK;
}    

MunitResult test_job_copy_heap(const MunitParameter params[], void* fixture) {
    pid_t pid = getpid();
    job_t src;
    
    for (int i = 0; i < TEST_LABELS; i++) {
        set_test_job(&src, pid, i, i);
        job_t* cpy = job_copy(NULL, &src);
        assert_ptr_not_equal(cpy, &src);
        assert_ptr_not_equal(cpy->label, src.label);
        
        munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_ITER_FMT, 
            __LINE__ + 1,  "assert_valid_job", i);
        assert_valid_job(cpy, pid, i, i);
        free(cpy);
    }
    
    return MUNIT_OK;
}

MunitResult test_job_copy_identity(const MunitParameter params[], 
    void* fixture) {
    pid_t pid = getpid();
    job_t src;
    
    for (int i = 0; i < TEST_LABELS; i++) {
        set_test_job(&src, pid, i, i);
        job_t* cpy = job_copy(&src, &src);
        assert_ptr_equal(cpy, &src);

        munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_ITER_FMT, 
            __LINE__ + 1,  "assert_valid_job", i);
        assert_valid_job(cpy, pid, i, i);
    }
    
    job_t eq2src;
    
    for (int i = 0; i < TEST_LABELS; i++) {
        set_test_job(&src, pid, i, i);
        set_test_job(&eq2src, pid, i, i);
        assert_int(eq2src.pid, ==, src.pid);
        assert_int(eq2src.id, ==, src.id);
        assert_string_equal(eq2src.label, src.label);
        
        job_t* cpy = job_copy(&eq2src, &src);
        
        assert_ptr_equal(cpy, &eq2src);
        assert_ptr_equal(cpy->label, eq2src.label);
        assert_ptr_not_equal(cpy, &src);
        assert_ptr_not_equal(cpy->label, src.label);
        
        munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_ITER_FMT, 
            __LINE__ + 1,  "assert_valid_job", i);
        assert_valid_job(cpy, pid, i, i);
    }      
    
    return MUNIT_OK;
}

MunitResult test_job_copy_null(const MunitParameter params[], 
    void* fixture) {
    job_t dst;

    assert_null(job_copy(&dst, NULL));
    
    assert_null(job_copy(NULL, NULL));
    
    return MUNIT_OK;
}

MunitResult test_job_init_stack(const MunitParameter params[], void* fixture) {
    pid_t pid = getpid();
    job_t job;

    job_init(&job);
    
    munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_FMT, __LINE__ + 1,
        "assert_init_job");
    assert_init_job(&job);
    
    for (int i = 0; i < TEST_LABELS; i++) {
        set_test_job(&job, pid, i, i);
        job_init(&job);
        
        munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_ITER_FMT, 
            __LINE__ + 1,  "assert_init_job", i);
        assert_init_job(&job) ;
    }
    
    return MUNIT_OK;
}    

MunitResult test_job_init_heap(const MunitParameter params[], void* fixture) {
    pid_t pid = getpid();
    job_t* job = (job_t*) malloc(sizeof(job_t));

    job_init(job);    

    munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_FMT, __LINE__ + 1,
        "assert_init_job");
    assert_init_job(job);
    
    free(job);
    
    for (int i = 0; i < TEST_LABELS; i++) {
        job = (job_t*) malloc(sizeof(job_t));
        set_test_job(job, pid, i, i);
        job_init(job);

        munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_ITER_FMT, 
            __LINE__ + 1,  "assert_init_job", i);
        assert_init_job(job);
        
        free(job);
    }
    
    return MUNIT_OK;
}    

MunitResult test_job_init_null(const MunitParameter params[], void* fixture) {
    job_init(NULL);
    
    return MUNIT_OK;
}

MunitResult test_job_is_equal(const MunitParameter params[], 
    void* fixture) { 
    job_t j1;
    job_t j2;
    
    for (int i = 0; i < TEST_LABELS; i++) {
        set_test_job(&j1, i + 1, i + 2, i);
        set_test_job(&j2, i + 1, i + 2, i);
        
        munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_ITER_FMT, 
            __LINE__ + 1,  "assert_job_equalities", i);
        assert_job_equalities(&j1, &j2);
        
        munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_ITER_FMT, 
            __LINE__ + 1,  "assert_job_equalities", i);
        assert_job_equalities(&j2, &j1);   
    }
    
    return MUNIT_OK;
}   

MunitResult test_job_is_equal_init(const MunitParameter params[], 
    void* fixture) { 
    job_t j1;
    job_t j2;
    
    for (int i = 0; i < 1; i++) {
        set_test_job(&j1, 0, 0, -1);
        set_test_job(&j2, 0, 0, -1);

        munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_ITER_FMT, 
            __LINE__ + 1,  "assert_init_job", i);
        assert_init_job(&j1);
        assert_init_job(&j2);
        assert_true(job_is_equal(&j1, &j2));
        assert_true(job_is_equal(&j2, &j1));
        
        set_test_job(&j1, i, i, i);
        assert_false(job_is_equal(&j1, &j2));
        assert_false(job_is_equal(&j2, &j1));
    }
    
    j1.pid = 0;
    j1.id = 0;
    j1.label[0] = 0;
    
    assert_true(job_is_equal(&j1, &j2));
    assert_true(job_is_equal(&j2, &j1));

    return MUNIT_OK;
}   

MunitResult test_job_is_equal_null(const MunitParameter params[], 
    void* fixture) { 
    assert_true(job_is_equal(NULL, NULL));

    job_t job;
    
    for (int i = 0; i < TEST_LABELS; i++) {
        set_test_job(&job, i + 1, i + 2, i);
        
        assert_false(job_is_equal(&job, NULL));
        assert_false(job_is_equal(NULL, &job));
    }     

    return MUNIT_OK;
}   

MunitResult test_job_set_stack(const MunitParameter params[], void* fixture) {
    pid_t pid = getpid();
    job_t job;
    
    for (int i = 0; i < TEST_LABELS; i++) {
        job_t* set_job = job_set(&job, pid, i, label_in[i]);
        assert_ptr_equal(set_job, &job);

        munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_ITER_FMT, 
            __LINE__ + 1,  "assert_valid_job", i);
        assert_valid_job(set_job, pid, i, i);
    }
    
    return MUNIT_OK;
}    

MunitResult test_job_set_heap(const MunitParameter params[], void* fixture) {
    pid_t pid = getpid();
    
    for (int i = 0; i < TEST_LABELS; i++) {
        job_t* job = (job_t*) malloc(sizeof(job_t));
        job_t* set_job = job_set(job, pid, i, label_in[i]);
        assert_ptr_equal(set_job, job);

        munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_ITER_FMT, 
            __LINE__ + 1,  "assert_valid_job", i);
        assert_valid_job(set_job, pid, i, i);
        
        free(job);
    }
    
    return MUNIT_OK;
}    

MunitResult test_job_set_null(const MunitParameter params[], void* fixture) {
    pid_t pid = getpid();

    assert_null(job_set(NULL, pid, 1, label_in[0]));
    
    job_t job;
    job_t* set_job = job_set(&job, pid, 1, NULL);
    assert_ptr_equal(set_job, &job);

    munit_logf(MUNIT_LOG_INFO, NCL_INFO_CALL_FMT, 
            __LINE__ + 1,  "assert_valid_job");
    assert_valid_job(set_job, pid, 1, 0);
    
    return MUNIT_OK;
}

