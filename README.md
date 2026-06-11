# Operating System Simulator with Chat Server

A Linux-based C application built for the Operating Systems (CSL-320) course at 
Bahria University. It combines four OS concept modules under a single command-line 
menu interface.

## Modules

### 🕐 CPU Scheduling
Implements four scheduling algorithms on the same process set:
- **FCFS** – First Come First Served
- **SJF** – Shortest Job First (greedy, lowest waiting time)
- **Priority** – Priority-based scheduling
- **Round Robin** – Preemptive with configurable time quantum

### 🧵 Thread Programming
- Demonstrates concurrent execution using POSIX threads
- Shows race conditions (unprotected shared counter)
- Fixes them with mutex locking
- Thread lifecycle demo with creation-vs-finish order

### 🔒 Process Synchronization
- **Producer-Consumer** – Circular buffer (size 5) with mutex + condition variables
- **Bank Account Simulation** – 8 concurrent threads using semaphores

### 💬 Chat Server
- TCP server on port 8080
- Per-client detached threads with shared client list (mutex-protected)
- Commands: `/list`, `/msg <user>`, `/quit`, and broadcast messaging

## Build
gcc main.c -o os_simulator -lpthread

## Tech
- **Language:** C
- **Platform:** Linux
- **Concurrency:** POSIX Threads (pthreads)
- **Networking:** TCP Sockets
