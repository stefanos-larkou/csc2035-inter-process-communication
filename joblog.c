/*
 * Replace the following string of 0s with your student number
 * c0034805
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include "joblog.h"

// DO NOT EDIT the following definitions of log string constant values */
const char* JOBLOG_PATH = "./out";
const char* JOBLOG_NAME_FMT = "%s/%s%07d.txt";
const char* JOBLOG_ENTRY_FMT = "pid:%07d,id:%05d,label:%s\n";

/* 
 * DO NOT EDIT the new_log_name function. It is a private helper 
 * function provided for you to create a log name from a process 
 * descriptor for use when reading, writing and deleting a log file.
 * 
 * You must work out what the function does in order to use it properly
 * and to clean up after use.
 *
 * See JOBLOG_NAME_FMT above for the form of log names.
 */
static char* new_log_name(proc_t* proc) {
    if (!proc)
        return NULL;

    char* log_name;
            
    asprintf(&log_name, JOBLOG_NAME_FMT, JOBLOG_PATH, proc->type_label,
        proc->id);

    return log_name;
}

/* 
 * DO NOT EDIT the joblog_init function.
 */
int joblog_init(proc_t* proc) {
    if (!proc) {
        errno = EINVAL;
        return -1;
    }
        
    int r = 0;
    if (proc->is_init) {
        struct stat sb;
    
        if (stat(JOBLOG_PATH, &sb) != 0) {
            errno = 0;
            r = mkdir(JOBLOG_PATH, 0777);
        }  else if (!S_ISDIR(sb.st_mode)) {
            unlink(JOBLOG_PATH);
            errno = 0;
            r = mkdir(JOBLOG_PATH, 0777);
        }
    }

    joblog_delete(proc);    // in case log exists for proc
    
    return r;
}

/* 
 * TODO: you must implement this function.
 * Hints:
 * - you have to go to the beginning of the line represented
 *      by entry_num to read the required entry
 * - see malloc and calloc system library functions for dynamic allocation, 
 *      and the documentation in joblog.h for when to do dynamic allocation
 */
char* joblog_read_entry(proc_t* proc, int entry_num, char* buf) {
    int init_errno = errno;

    if(!proc){
        errno = init_errno;
        return NULL;
    }

    char* logpath = new_log_name(proc);

    if(!logpath){
        errno = init_errno;
        return NULL;
    }

    FILE* logfile = fopen(logpath, "r");

    if(!logfile){
        errno = init_errno;
        return NULL;
    }

    int lines = 0;
    char c;
    while(!feof(logfile)){
        c = fgetc(logfile);
        if(c == '\n'){
            lines++;
        }
    }

    if(entry_num < 0 || entry_num > lines - 1){
        fclose(logfile);
        errno = init_errno;
        return NULL;
    }

    int error = fseek(logfile, entry_num * JOBLOG_ENTRY_SIZE, SEEK_SET);

    if(error == -1){
        return NULL;
    }

    if(buf){
        if(!fgets(buf, JOBLOG_ENTRY_SIZE, logfile)){
            return NULL;
        }
        buf[JOBLOG_ENTRY_SIZE - 1] = '\0';

        fclose(logfile);
        return buf;
    }

    char* new_buf = (char*) malloc(sizeof(char) * JOBLOG_ENTRY_SIZE);

    if(!fgets(new_buf, JOBLOG_ENTRY_SIZE, logfile)){
        return NULL;
    }
    new_buf[JOBLOG_ENTRY_SIZE - 1] = '\0';

    fclose(logfile);
    return new_buf;
}

/* 
 * TODO: you must implement this function.
 * Hints:
 * - look at the documentation of the format of a log entry in joblog.h
 * - remember new entries are appended to a log file
 */
void joblog_write_entry(proc_t* proc, job_t* job) {
    int init_errno = errno;

    if(!proc || !job){
        errno = init_errno;
        return;
    }

    char* logpath = new_log_name(proc);

    if(!logpath){
        errno = init_errno;
        return;
    }

    FILE* logfile = fopen(logpath, "a");

    if(!logfile){
        errno = init_errno;
        return;
    }

    char* log_entry;
    int err = asprintf(&log_entry, JOBLOG_ENTRY_FMT, job->pid, job->id, job->label);

    if(err == -1){
        errno = init_errno;
        return;
    }

    fprintf(logfile, "%s", log_entry);    
    fclose(logfile);
}

/* 
 * TODO: you must implement this function.
 */
void joblog_delete(proc_t* proc) {
    int init_errno = errno;

    char* logpath = new_log_name(proc);

    if(remove(logpath) != 0){
        errno = init_errno;
    }
}