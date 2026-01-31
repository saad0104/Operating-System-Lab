# 📂 Operating Systems Lab(CSE512)

##  Directory Overview
This repository contains a collection of **20 C programs** demonstrating core Operating System concepts. The experiments range from basic system calls and UNIX utilities to complex simulations of CPU scheduling, Deadlock handling, Memory management, and File systems. Each program is designed to run in a Linux/POSIX environment, using standard system libraries and CLI arguments.

---

##  Table of Contents & File Manifest

| Exp # | Experiment Name | Source File | Executable Name |
| :--- | :--- | :--- | :--- |
| **01** | Permission Calculator | `exp_01.c` | `permcalc` |
| **02** | POSIX File Copy | `exp_02.c` | `fdcopy` |
| **03** | Directory Report | `exp_03.c` | `dirreport` |
| **04** | grep-lite | `exp_04.c` | `greplite` |
| **05** | Process Spawner | `exp_05.c` | `spawnwait` |
| **06** | Timeout Supervisor | `exp_06.c` | `timeoutwrap` |
| **07** | Pipe Chain | `exp_07.c` | `pipechain` |
| **08** | Shared Memory Counter | `exp_08.c` | `shmcounter` |
| **09** | Threaded Sum | `exp_09.c` | `thrsum` |
| **10** | Producer-Consumer | `exp_10.c` | `pcbuf` |
| **11** | FCFS & SJF Simulator | `exp_11.c` | `schedsim1` |
| **12** | Round Robin Simulator | `exp_12.c` | `schedsim2` |
| **13** | Priority Scheduler | `exp_13.c` | `schedprio` |
| **14** | Banker's Algorithm | `exp_14.c` | `banker` |
| **15** | Deadlock Detection | `exp_15.c` | `wfgcheck` |
| **16** | Memory Allocation | `exp_16.c` | `memfit` |
| **17** | Page Translation | `exp_17.c` | `pagetrans` |
| **18** | Page Replacement | `exp_18.c` | `pagerepl` |
| **19** | File Allocation | `exp_19.c` | `filealloc` |
| **20** | Disk Scheduling | `exp_20.c` | `disksched` |

---

##  Prerequisites

To run these codes, you need:
1.  **Operating System:** Linux (Ubuntu/Debian recommended) or macOS. Windows users should use WSL (Windows Subsystem for Linux).
2.  **Compiler:** `gcc` (GNU Compiler Collection).
3.  **Libraries:** Standard C Library, plus `pthread` and `rt` for Experiments 8, 9, and 10.

**Compilation Commands:**
* **Standard:** `gcc filename.c -o filename`
* **For Exp 8, 9, 10:** `gcc filename.c -o filename -lpthread -lrt`

---

##  Execution Guide

### Part 1: System Utilities & Process Control
*These programs take direct command-line arguments.*

**1. Permission Calculator (`permcalc`)**
* *Desc:* Calculates effective permissions from octal mode and umask.
* *Run:* `./permcalc --mode 0644 --umask 0022`

**2. File Copy (`fdcopy`)**
* *Desc:* Copies file data using POSIX I/O and calculates CRC32 checksum.
* *Run:* `./fdcopy --src input.txt --dst output.bin`

**3. Directory Report (`dirreport`)**
* *Desc:* Lists directory contents with metadata (size, type).
* *Run:* `./dirreport --path ./test_dir --sort size`

**4. grep-lite (`greplite`)**
* *Desc:* Searches for a text pattern inside multiple files.
* *Run:* `./greplite --pattern "TODO" --files main.c,utils.c`

**5. Process Spawner (`spawnwait`)**
* *Desc:* Spawns child processes sequentially and reports exit status.
* *Run:* `./spawnwait --cmd /bin/echo --args "Hello" --repeat 3`

**6. Timeout Supervisor (`timeoutwrap`)**
* *Desc:* Runs a command and kills it if it exceeds the time limit.
* *Run:* `./timeoutwrap --seconds 2 --cmd /bin/sleep --args 5`

**7. Pipe Chain (`pipechain`)**
* *Desc:* Connects three commands: Producer -> Filter -> Consumer.
* *Run:* `./pipechain --producer /bin/ls --filter /bin/grep --consumer /bin/wc`

### Part 2: IPC & Concurrency
*These require the pthread linker flag.*

**8. Shared Memory Counter (`shmcounter`)**
* *Desc:* Uses Shared Memory and Semaphores to sync a counter across processes.
* *Run:* `./shmcounter --procs 4 --iters 100 --name test1`

**9. Threaded Sum (`thrsum`)**
* *Desc:* multi-threaded reduction to sum numbers 1 to N.
* *Run:* `./thrsum --threads 4 --n 100`

**10. Producer-Consumer (`pcbuf`)**
* *Desc:* Bounded buffer simulation using mutexes and semaphores.
* *Run:* `./pcbuf --buf 5 --producers 2 --consumers 2 --items 20`

### Part 3: Simulations (Input via STDIN)
*These accept configuration data piped via `printf` or `< file.txt`.*

**11. CPU Scheduling I (`schedsim1`)**
* *Desc:* Simulates FCFS and SJF scheduling.
* *Input:* `pid,arrival,burst`
* *Run:* `printf "P1,0,5\nP2,2,2" | ./schedsim1`

**12. CPU Scheduling II (`schedsim2`)**
* *Desc:* Simulates Round Robin scheduling.
* *Input:* `pid,arrival,burst`
* *Run:* `printf "P1,0,5\nP2,2,2" | ./schedsim2 --q 2`

**13. Priority Scheduling (`schedprio`)**
* *Desc:* Simulates Priority scheduling with aging.
* *Input:* `pid,arrival,burst,priority`
* *Run:* `printf "A,0,4,10\nB,1,2,0" | ./schedprio`

**14. Banker's Algorithm (`banker`)**
* *Desc:* Checks for safe state to avoid deadlocks.
* *Input:* Proc Count, Res Count, Alloc Matrix, Max Matrix, Avail Vector.
* *Run:* `printf "3 2\n1 0\n0 1\n1 1\n2 0\n1 2\n1 1\n1 1" | ./banker`

**15. Deadlock Detection (`wfgcheck`)**
* *Desc:* Detects cycles in a Wait-For Graph (WFG).
* *Input:* Node Count, Edge Count, Edges (u v).
* *Run:* `printf "3 3\n0 1\n1 2\n2 1" | ./wfgcheck`

**16. Memory Allocation (`memfit`)**
* *Desc:* Simulates First-Fit, Best-Fit, and Worst-Fit allocation.
* *Input:* Block Count, Block Sizes, Proc Count, Proc Sizes.
* *Run:* `printf "3\n10 20 30\n2\n5 15" | ./memfit`

**17. Page Translation (`pagetrans`)**
* *Desc:* Simulates VA to PA translation using Page Table + TLB.
* *Input:* PT size, PT Entries, Query Count, Virtual Addresses.
* *Run:* `printf "2\n0 5 1\n1 9 0\n1\n300" | ./pagetrans --pagesize 256`

**18. Page Replacement (`pagerepl`)**
* *Desc:* Simulates FIFO, LRU, and OPT page replacement.
* *Input:* Reference String Length, Page Numbers.
* *Run:* `printf "5\n1 2 3 1 2" | ./pagerepl --frames 3`

**19. File Allocation (`filealloc`)**
* *Desc:* Simulates Contiguous, Linked, and Indexed file allocation.
* *Input:* Total Blocks, Free List, File Requests.
* *Run:* `printf "10\n3\n0 1 2\n1\nA 2" | ./filealloc`

**20. Disk Scheduling (`disksched`)**
* *Desc:* Simulates FCFS, SSTF, SCAN, and C-SCAN disk arm movement.
* *Input:* Request Count, Cylinder Numbers.
* *Run:* `printf "5\n55 58 39 18 90" | ./disksched --max 199 --start 50 --dir right`

---

## ✨ Brief Highlights
* **System Calls:** Deep dive into `fork()`, `exec()`, `waitpid()`, and `signal()` handling.
* **Synchronization:** Practical implementation of Mutex locks and Semaphores to solve race conditions.
* **Algorithms:** faithful implementations of standard OS algorithms (Banker's, LRU, Round Robin, SCAN) illustrating how operating systems manage resources internally.
* **Error Handling:** Robust error codes (e.g., `E_USAGE`, `E_RANGE`) are implemented across all tools for deterministic behavior.

  
