#ifndef CPU_SCHEDULING_H
#define CPU_SCHEDULING_H

#include "../include/common.h"

/* ── helpers ── */
static void reset_processes(Process *p, int n) {
    for (int i = 0; i < n; i++) {
        p[i].remaining_time  = p[i].burst_time;
        p[i].waiting_time    = 0;
        p[i].turnaround_time = 0;
        p[i].completion_time = 0;
        p[i].finished        = 0;
    }
}

static void print_gantt_bar(int *order, int len) {
    printf("\nGantt Chart:\n|");
    for (int i = 0; i < len; i++) printf(" P%d |", order[i]);
    printf("\n");
}

static void print_results(Process *p, int n, ScheduleResult *r) {
    printf("\n%-6s %-8s %-8s %-12s %-12s %-14s\n",
           "PID","Burst","Arrive","Wait Time","Turnaround","Completion");
    print_separator();
    double tw = 0, tt = 0;
    for (int i = 0; i < n; i++) {
        printf("P%-5d %-8d %-8d %-12d %-12d %-14d\n",
               p[i].pid, p[i].burst_time, p[i].arrival_time,
               p[i].waiting_time, p[i].turnaround_time, p[i].completion_time);
        tw += p[i].waiting_time;
        tt += p[i].turnaround_time;
    }
    r->avg_waiting_time    = tw / n;
    r->avg_turnaround_time = tt / n;
    r->total_processes     = n;
    print_separator();
    printf(CLR_GREEN "Avg Waiting Time   : %.2f\n" CLR_RESET, r->avg_waiting_time);
    printf(CLR_GREEN "Avg Turnaround Time: %.2f\n" CLR_RESET, r->avg_turnaround_time);
}

/* ── FCFS ── */
static void sort_by_arrival(Process *p, int n) {
    for (int i = 0; i < n-1; i++)
        for (int j = 0; j < n-i-1; j++)
            if (p[j].arrival_time > p[j+1].arrival_time) {
                Process tmp = p[j]; p[j] = p[j+1]; p[j+1] = tmp;
            }
}

void fcfs(Process *p, int n, ScheduleResult *r) {
    print_header("FCFS Scheduling");
    reset_processes(p, n);
    sort_by_arrival(p, n);

    int time = 0;
    int gantt[MAX_PROCESSES]; int gl = 0;
    for (int i = 0; i < n; i++) {
        if (time < p[i].arrival_time) time = p[i].arrival_time;
        p[i].waiting_time    = time - p[i].arrival_time;
        time                += p[i].burst_time;
        p[i].completion_time = time;
        p[i].turnaround_time = p[i].completion_time - p[i].arrival_time;
        gantt[gl++] = p[i].pid;
    }
    print_gantt_bar(gantt, gl);
    print_results(p, n, r);
}

/* ── SJF (Non-Preemptive) ── */
void sjf(Process *p, int n, ScheduleResult *r) {
    print_header("SJF Non-Preemptive Scheduling");
    reset_processes(p, n);

    int time = 0, done = 0;
    int gantt[MAX_PROCESSES]; int gl = 0;
    while (done < n) {
        int idx = -1, min_b = 1e9;
        for (int i = 0; i < n; i++)
            if (!p[i].finished && p[i].arrival_time <= time && p[i].burst_time < min_b) {
                min_b = p[i].burst_time; idx = i;
            }
        if (idx == -1) { time++; continue; }
        p[idx].waiting_time    = time - p[idx].arrival_time;
        time                  += p[idx].burst_time;
        p[idx].completion_time = time;
        p[idx].turnaround_time = p[idx].completion_time - p[idx].arrival_time;
        p[idx].finished        = 1;
        gantt[gl++] = p[idx].pid;
        done++;
    }
    print_gantt_bar(gantt, gl);
    print_results(p, n, r);
}

/* ── Round Robin ── */
void round_robin(Process *p, int n, int quantum, ScheduleResult *r) {
    print_header("Round Robin Scheduling");
    char title[64];
    sprintf(title, "Round Robin Scheduling (Quantum=%d)", quantum);
    print_header(title);
    reset_processes(p, n);
    sort_by_arrival(p, n);

    int time = 0, done = 0;
    int gantt[MAX_PROCESSES * 10]; int gl = 0;
    while (done < n) {
        int found = 0;
        for (int i = 0; i < n; i++) {
            if (!p[i].finished && p[i].arrival_time <= time && p[i].remaining_time > 0) {
                found = 1;
                gantt[gl++] = p[i].pid;
                int exec = (p[i].remaining_time < quantum) ? p[i].remaining_time : quantum;
                p[i].remaining_time -= exec;
                time += exec;
                if (p[i].remaining_time == 0) {
                    p[i].finished        = 1;
                    p[i].completion_time = time;
                    p[i].turnaround_time = p[i].completion_time - p[i].arrival_time;
                    p[i].waiting_time    = p[i].turnaround_time - p[i].burst_time;
                    done++;
                }
            }
        }
        if (!found) time++;
    }
    print_gantt_bar(gantt, gl > 20 ? 20 : gl);
    if (gl > 20) printf("  ... (truncated)\n");
    print_results(p, n, r);
}

/* ── Priority (Non-Preemptive, lower number = higher priority) ── */
void priority_scheduling(Process *p, int n, ScheduleResult *r) {
    print_header("Priority Scheduling (Non-Preemptive)");
    reset_processes(p, n);

    int time = 0, done = 0;
    int gantt[MAX_PROCESSES]; int gl = 0;
    while (done < n) {
        int idx = -1, hi = 1e9;
        for (int i = 0; i < n; i++)
            if (!p[i].finished && p[i].arrival_time <= time && p[i].priority < hi) {
                hi = p[i].priority; idx = i;
            }
        if (idx == -1) { time++; continue; }
        p[idx].waiting_time    = time - p[idx].arrival_time;
        time                  += p[idx].burst_time;
        p[idx].completion_time = time;
        p[idx].turnaround_time = p[idx].completion_time - p[idx].arrival_time;
        p[idx].finished        = 1;
        gantt[gl++] = p[idx].pid;
        done++;
    }
    print_gantt_bar(gantt, gl);
    print_results(p, n, r);
}

/* ── Interactive input helper ── */
void input_processes(Process *p, int *n) {
    printf("Enter number of processes (1-%d): ", MAX_PROCESSES);
    scanf("%d", n);
    if (*n < 1 || *n > MAX_PROCESSES) { *n = 3; printf("Using default 3.\n"); }
    for (int i = 0; i < *n; i++) {
        p[i].pid = i + 1;
        sprintf(p[i].name, "P%d", i+1);
        printf("  P%d Burst Time : ", i+1); scanf("%d", &p[i].burst_time);
        printf("  P%d Arrival Time: ", i+1); scanf("%d", &p[i].arrival_time);
        printf("  P%d Priority   : ", i+1); scanf("%d", &p[i].priority);
    }
}

void run_cpu_scheduling_menu(void) {
    Process p[MAX_PROCESSES];
    ScheduleResult r;
    int n, choice;

    print_header("CPU Scheduling Simulator");
    printf("Use sample processes? (1=Yes / 0=No): ");
    int use_sample; scanf("%d", &use_sample);

    if (use_sample) {
        n = 5;
        int bt[] = {6, 8, 7, 3, 4};
        int at[] = {0, 1, 2, 3, 4};
        int pr[] = {3, 2, 4, 1, 2};
        for (int i = 0; i < n; i++) {
            p[i].pid = i+1; sprintf(p[i].name,"P%d",i+1);
            p[i].burst_time = bt[i]; p[i].arrival_time = at[i]; p[i].priority = pr[i];
        }
        printf("Loaded 5 sample processes.\n");
    } else {
        input_processes(p, &n);
    }

    do {
        printf("\n" CLR_BOLD "  [1] FCFS\n  [2] SJF\n  [3] Round Robin\n  [4] Priority\n  [0] Back\n" CLR_RESET);
        printf("Choice: "); scanf("%d", &choice);
        switch (choice) {
            case 1: fcfs(p, n, &r); break;
            case 2: sjf(p, n, &r); break;
            case 3: {
                int q; printf("Time Quantum: "); scanf("%d", &q);
                round_robin(p, n, q, &r); break;
            }
            case 4: priority_scheduling(p, n, &r); break;
        }
    } while (choice != 0);
}

#endif /* CPU_SCHEDULING_H */
