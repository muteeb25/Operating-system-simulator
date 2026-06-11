#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include "../include/common.h"

/* ═══════════════════════════════════════════════════
   PRODUCER-CONSUMER  (mutex + condition variables)
   ═══════════════════════════════════════════════════ */

static SharedBuffer shared_buf;
static int pc_done = 0;

static void buf_init(void) {
    shared_buf.in = shared_buf.out = shared_buf.count = 0;
    pthread_mutex_init(&shared_buf.mutex, NULL);
    pthread_cond_init(&shared_buf.not_full,  NULL);
    pthread_cond_init(&shared_buf.not_empty, NULL);
    pc_done = 0;
}

static void buf_destroy(void) {
    pthread_mutex_destroy(&shared_buf.mutex);
    pthread_cond_destroy(&shared_buf.not_full);
    pthread_cond_destroy(&shared_buf.not_empty);
}

static void *producer(void *arg) {
    int items = *(int *)arg;
    for (int i = 1; i <= items; i++) {
        pthread_mutex_lock(&shared_buf.mutex);
        while (shared_buf.count == BUFFER_SIZE)
            pthread_cond_wait(&shared_buf.not_full, &shared_buf.mutex);

        shared_buf.buffer[shared_buf.in] = i;
        shared_buf.in = (shared_buf.in + 1) % BUFFER_SIZE;
        shared_buf.count++;
        printf("  " CLR_GREEN "[Producer]" CLR_RESET " Produced item %2d  (buffer: %d/%d)\n",
               i, shared_buf.count, BUFFER_SIZE);

        pthread_cond_signal(&shared_buf.not_empty);
        pthread_mutex_unlock(&shared_buf.mutex);
        usleep(80000);
    }
    pthread_mutex_lock(&shared_buf.mutex);
    pc_done = 1;
    pthread_cond_broadcast(&shared_buf.not_empty);
    pthread_mutex_unlock(&shared_buf.mutex);
    return NULL;
}

static void *consumer(void *arg) {
    (void)arg;
    while (1) {
        pthread_mutex_lock(&shared_buf.mutex);
        while (shared_buf.count == 0 && !pc_done)
            pthread_cond_wait(&shared_buf.not_empty, &shared_buf.mutex);
        if (shared_buf.count == 0 && pc_done) {
            pthread_mutex_unlock(&shared_buf.mutex);
            break;
        }
        int item = shared_buf.buffer[shared_buf.out];
        shared_buf.out = (shared_buf.out + 1) % BUFFER_SIZE;
        shared_buf.count--;
        printf("  " CLR_RED "[Consumer]" CLR_RESET " Consumed item %2d  (buffer: %d/%d)\n",
               item, shared_buf.count, BUFFER_SIZE);

        pthread_cond_signal(&shared_buf.not_full);
        pthread_mutex_unlock(&shared_buf.mutex);
        usleep(120000);
    }
    return NULL;
}

static void run_producer_consumer(void) {
    print_header("Producer-Consumer (mutex + condvar)");
    buf_init();
    int items = 10;
    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer, &items);
    pthread_create(&cons, NULL, consumer, NULL);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    buf_destroy();
    printf(CLR_GREEN "\nProducer-Consumer completed — no race conditions.\n" CLR_RESET);
}

/* ═══════════════════════════════════════════════════
   BANK ACCOUNT  (semaphore)
   ═══════════════════════════════════════════════════ */

static BankAccount bank;

static void bank_init(double initial) {
    bank.balance = initial;
    bank.transaction_count = 0;
    sem_init(&bank.sem, 0, 1);
}

static void bank_destroy(void) {
    sem_destroy(&bank.sem);
}

static void *bank_transaction(void *arg) {
    double amount = *(double *)arg;
    sem_wait(&bank.sem);
    double old = bank.balance;
    usleep(10000); /* simulate processing delay */
    bank.balance += amount;
    bank.transaction_count++;
    printf("  [Txn %2d]  %s%.2f   Balance: %.2f → %.2f\n",
           bank.transaction_count,
           amount >= 0 ? "+" : "",
           amount, old, bank.balance);
    sem_post(&bank.sem);
    return NULL;
}

static void run_bank_simulation(void) {
    print_header("Bank Account Simulation (semaphore)");
    bank_init(1000.00);
    printf("  Initial balance: %.2f\n\n", bank.balance);

    double amounts[] = {200, -150, 500, -300, 100, -75, 250, -400};
    int nt = 8;
    pthread_t threads[8];
    for (int i = 0; i < nt; i++)
        pthread_create(&threads[i], NULL, bank_transaction, &amounts[i]);
    for (int i = 0; i < nt; i++)
        pthread_join(threads[i], NULL);

    bank_destroy();
    printf(CLR_GREEN "\nFinal Balance: %.2f  (%d transactions, no deadlocks)\n" CLR_RESET,
           bank.balance, bank.transaction_count);
}

/* ── Menu ── */
void run_synchronization_menu(void) {
    int choice;
    do {
        print_header("Synchronization Module");
        printf(CLR_BOLD
               "  [1] Producer-Consumer (mutex + condition variables)\n"
               "  [2] Bank Account Simulation (semaphore)\n"
               "  [0] Back\n" CLR_RESET);
        printf("Choice: "); scanf("%d", &choice);
        switch (choice) {
            case 1: run_producer_consumer(); break;
            case 2: run_bank_simulation();   break;
        }
    } while (choice != 0);
}

#endif /* SYNCHRONIZATION_H */
