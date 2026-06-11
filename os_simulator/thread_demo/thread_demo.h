#ifndef THREAD_DEMO_H
#define THREAD_DEMO_H

#include "../include/common.h"

/* ── Shared counter for race condition demo ── */
static long counter_unsafe = 0;
static long counter_safe   = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
static int ITER = 100000;

static void *increment_unsafe(void *arg) {
    (void)arg;
    for (int i = 0; i < ITER; i++) counter_unsafe++;
    return NULL;
}

static void *increment_safe(void *arg) {
    (void)arg;
    for (int i = 0; i < ITER; i++) {
        pthread_mutex_lock(&counter_mutex);
        counter_safe++;
        pthread_mutex_unlock(&counter_mutex);
    }
    return NULL;
}

/* ── Thread lifecycle demo ── */
typedef struct { int id; int sleep_ms; } ThreadArg;

static void *thread_lifecycle(void *arg) {
    ThreadArg *ta = (ThreadArg *)arg;
    printf("  [Thread %d] Started\n", ta->id);
    usleep(ta->sleep_ms * 1000);
    printf("  [Thread %d] Finishing after %dms\n", ta->id, ta->sleep_ms);
    return (void *)(long)ta->id;
}

/* ── Runner ── */
void run_thread_demo(void) {
    print_header("Thread Programming Demo");

    /* --- Race condition --- */
    printf(CLR_YELLOW "\n1. Race Condition Demo\n" CLR_RESET);
    printf("   Two threads each increment a counter %d times without locking.\n", ITER);
    counter_unsafe = 0;
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment_unsafe, NULL);
    pthread_create(&t2, NULL, increment_unsafe, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("   Expected : %d\n", 2 * ITER);
    printf("   Got      : %ld  %s\n", counter_unsafe,
           counter_unsafe == 2 * ITER ? CLR_GREEN "(correct!)" CLR_RESET
                                      : CLR_RED   "(data race!)" CLR_RESET);

    /* --- Mutex fix --- */
    printf(CLR_YELLOW "\n2. Mutex-Protected Counter\n" CLR_RESET);
    counter_safe = 0;
    pthread_create(&t1, NULL, increment_safe, NULL);
    pthread_create(&t2, NULL, increment_safe, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("   Expected : %d\n", 2 * ITER);
    printf("   Got      : %ld  %s\n", counter_safe,
           counter_safe == 2 * ITER ? CLR_GREEN "(correct!)" CLR_RESET
                                    : CLR_RED   "(error!)"   CLR_RESET);

    /* --- Thread lifecycle --- */
    printf(CLR_YELLOW "\n3. Thread Lifecycle (4 threads, varying durations)\n" CLR_RESET);
    pthread_t threads[4];
    ThreadArg args[4] = {{1,100},{2,200},{3,50},{4,150}};
    for (int i = 0; i < 4; i++)
        pthread_create(&threads[i], NULL, thread_lifecycle, &args[i]);
    for (int i = 0; i < 4; i++) {
        void *ret;
        pthread_join(threads[i], &ret);
        printf("  [Thread %d] Joined — returned %ld\n", i+1, (long)ret);
    }
    printf(CLR_GREEN "\nAll threads completed.\n" CLR_RESET);
}

#endif /* THREAD_DEMO_H */
