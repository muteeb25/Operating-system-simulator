/*
 * ╔══════════════════════════════════════════════════════════════╗
 * ║   OS Simulator — Bahria University, Karachi Campus          ║
 * ║   Course: Operating Systems  (CSC-320)                      ║
 * ║   Group: Muteeb-ur-Rehman | Khaqan Aslam | Ahsan Raza       ║
 * ╚══════════════════════════════════════════════════════════════╝
 *
 * Compile:
 *   gcc -Wall -O2 main.c -lpthread -o os_sim
 *
 * Run:
 *   ./os_sim
 */

#include "cpu_scheduling/cpu_scheduling.h"
#include "thread_demo/thread_demo.h"
#include "synchronization/synchronization.h"
#include "chat_server/chat_server.h"

static void print_main_menu(void) {
    printf("\n"
           CLR_CYAN CLR_BOLD
           "  ┌─────────────────────────────────────────┐\n"
           "  │    OS Simulator  —  Main Menu           │\n"
           "  ├─────────────────────────────────────────┤\n"
           "  │  [1]  CPU Scheduling Simulator          │\n"
           "  │  [2]  Thread Programming Demo           │\n"
           "  │  [3]  Synchronization Module            │\n"
           "  │  [4]  Multithreaded Chat Server         │\n"
           "  │  [0]  Exit                              │\n"
           "  └─────────────────────────────────────────┘\n"
           CLR_RESET);
    printf("  Select module: ");
}

int main(void) {
    int choice;
    printf(CLR_BOLD CLR_CYAN
           "\n  ╔══════════════════════════════════════════╗\n"
           "  ║    OPERATING SYSTEM SIMULATOR            ║\n"
           "  ║    Bahria University — BSE-4(A)          ║\n"
           "  ╚══════════════════════════════════════════╝\n\n"
           CLR_RESET);

    do {
        print_main_menu();
        if (scanf("%d", &choice) != 1) { choice = -1; while(getchar()!='\n'); }

        switch (choice) {
            case 1: run_cpu_scheduling_menu();    break;
            case 2: run_thread_demo();             break;
            case 3: run_synchronization_menu();    break;
            case 4: run_chat_server();             break;
            case 0: printf(CLR_GREEN "\nGoodbye!\n\n" CLR_RESET); break;
            default: printf(CLR_RED "Invalid option.\n" CLR_RESET);
        }
    } while (choice != 0);

    return 0;
}
