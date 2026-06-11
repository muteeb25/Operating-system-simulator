#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_PROCESSES   20
#define MAX_THREADS     10
#define BUFFER_SIZE     5
#define MAX_CLIENTS     10
#define MAX_NAME_LEN    64
#define MAX_MSG_LEN     512
#define CHAT_PORT       8080

/* ── Process for CPU Scheduling ── */
typedef struct {
    int   pid;
    char  name[MAX_NAME_LEN];
    int   burst_time;
    int   arrival_time;
    int   priority;
    int   remaining_time;
    int   waiting_time;
    int   turnaround_time;
    int   completion_time;
    int   finished;
} Process;

/* ── Scheduler results ── */
typedef struct {
    double avg_waiting_time;
    double avg_turnaround_time;
    int    total_processes;
} ScheduleResult;

/* ── Shared buffer for Producer-Consumer ── */
typedef struct {
    int   buffer[BUFFER_SIZE];
    int   in, out, count;
    pthread_mutex_t mutex;
    pthread_cond_t  not_full;
    pthread_cond_t  not_empty;
} SharedBuffer;

/* ── Bank account for semaphore demo ── */
typedef struct {
    double balance;
    sem_t  sem;
    int    transaction_count;
} BankAccount;

/* ── Chat client info ── */
typedef struct {
    int   fd;
    char  username[MAX_NAME_LEN];
    int   active;
} ChatClient;

/* Colours */
#define CLR_RESET  "\033[0m"
#define CLR_BOLD   "\033[1m"
#define CLR_RED    "\033[31m"
#define CLR_GREEN  "\033[32m"
#define CLR_YELLOW "\033[33m"
#define CLR_CYAN   "\033[36m"
#define CLR_WHITE  "\033[37m"

static inline void print_header(const char *title) {
    printf("\n" CLR_CYAN CLR_BOLD
           "╔══════════════════════════════════════════════════╗\n"
           "║  %-48s║\n"
           "╚══════════════════════════════════════════════════╝\n"
           CLR_RESET, title);
}

static inline void print_separator(void) {
    printf(CLR_CYAN "──────────────────────────────────────────────────\n" CLR_RESET);
}

#endif /* COMMON_H */
