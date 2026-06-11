# Operating System Simulator
**Bahria University, Karachi Campus — BSE-4(A)**
**Course:** Operating Systems (CSC-320) | **Instructor:** Engr. Dr. Raazi

**Group Members**
| Name | ID | Role |
|---|---|---|
| Muteeb-ur-Rehman | 02-131242-037 | Team Lead |
| Khaqan Aslam | 02-131242-040 | Thread & Sync |
| Ahsan Raza | 02-131242-061 | Semaphore & Testing |

---

## Project Overview
A Linux-based OS Simulator written in **C** that demonstrates core OS concepts through a unified CLI application.

## Modules

### 1. CPU Scheduling (`cpu_scheduling/`)
| Algorithm | Type |
|---|---|
| FCFS | Non-preemptive, arrival-order |
| SJF | Non-preemptive, shortest burst first |
| Round Robin | Preemptive, configurable quantum |
| Priority | Non-preemptive, lower number = higher priority |

Outputs **Gantt chart**, **Waiting Time**, and **Turnaround Time** for all processes.

### 2. Thread Programming (`thread_demo/`)
- Demonstrates **race conditions** with two threads sharing an unprotected counter
- Fixes the race with **mutex locks** and shows the correct result
- Shows **thread lifecycle**: creation, concurrent execution, join

### 3. Synchronization (`synchronization/`)
- **Producer-Consumer** — mutex + condition variables, bounded buffer of size 5
- **Bank Account Simulation** — semaphore-protected deposits/withdrawals across 8 concurrent threads

### 4. Multithreaded Chat Server (`chat_server/`)
- TCP server on **port 8080**; each client handled by a dedicated thread
- Commands: `/list`, `/msg <user> <text>`, `/quit`
- Features: login, broadcast, private messaging, thread-safe client list

---

## Build & Run

### Requirements
- Linux (Ubuntu 20.04+ recommended)
- GCC with pthreads (`sudo apt install build-essential`)

### Compile
```bash
make
```
or manually:
```bash
gcc -Wall -O2 main.c -lpthread -o os_sim
```

### Run
```bash
./os_sim
```

### Chat Server Test
In one terminal:
```bash
./os_sim   # choose option 4
```
In other terminals:
```bash
telnet 127.0.0.1 8080
```

---

## Project Structure
```
os_simulator/
├── main.c                          # CLI entry point
├── Makefile
├── README.md
├── include/
│   └── common.h                    # Shared types, colours, macros
├── cpu_scheduling/
│   └── cpu_scheduling.h            # FCFS, SJF, RR, Priority
├── thread_demo/
│   └── thread_demo.h               # Race condition & mutex demo
├── synchronization/
│   └── synchronization.h           # Producer-Consumer, Bank Account
└── chat_server/
    └── chat_server.h               # TCP multithreaded chat
```

---

## Limitations
- User-space only (no kernel modules)
- No GUI (CLI only)
- Chat server exits on SIGINT (Ctrl-C)
