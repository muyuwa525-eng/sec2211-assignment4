Markdown# SEC 2211: System Programming and Computer Controls
## Practical Assignment 2 — Week 1 Checkpoint (Q1 & Q2)

**Course**: SEC 2211 — System Programming and Computer Controls  
**Institution**: Zambia University College of Technology (ZUT)  
**Environment**: Linux (Ubuntu / Kali Linux)  

---

## Overview

This repository contains the source code, experimental evidence, laboratory configurations, and demonstrations for **Week 1 (Questions 1 & 2)** of the SEC 2211 Practical Assignment.

The project covers fundamental Unix system programming, low-level I/O operations, process memory layouts, system call tracing, Discretionary Access Control (DAC), privilege management, and secure system design.

---

## Repository Structure

```text
sec2211-assignment4/
├── secinspect.c            # Source code for Question 1 I/O utility
├── test.txt                # Sample input file for file descriptor operations
├── secret.txt              # Restricted file for security testing (mode 0600)
├── whoami_ids.c            # C program to inspect Real UID vs. Effective UID
├── security-lab/           # Directory structure for Question 2 permissions lab
│   ├── public/             # World-readable public announcements (mode 644)
│   ├── private/            # Restrictive confidential storage (mode 600)
│   ├── logs/               # Dedicated logging folder (audit.log, mode 640)
│   └── application/        # Restricted application script directory (mode 700)
├── evidence/               # Experimental logs, strace captures, and terminal outputs
│   ├── strace_output.txt   # System call trace logs for secinspect
│   └── secinspect_output.txt # Output generated via dup2() redirection
└── README.md               # Repository documentation
Question 1: System-Level I/O & Process InspectionKey ObjectivesUnderstand the transition: $\text{C Program} \rightarrow \text{System Call} \rightarrow \text{Kernel} \rightarrow \text{File Descriptor} \rightarrow \text{I/O Resource}$.Observe low-level I/O primitives: open(), read(), write(), lseek(), stat()/fstat(), and close().Inspect process memory layout and compilation pipeline stages (gcc, readelf, gdb, /proc).Demonstrate I/O redirection using dup2().Perform kernel-level permission boundary testing.Compilation & ExecutionCompile the utility using standard gcc:Bashgcc -Wall -o secinspect secinspect.c
./secinspect test.txt
Key Demonstrations & Command ReferenceSystem Call Tracing (strace):Verify system call translation (openat, read, write, lseek, close):Bashstrace -o evidence/strace_output.txt ./secinspect test.txt
File Descriptor Inspection (/proc):Inspect active descriptors assigned by the kernel (Standard descriptors 0, 1, 2 + assigned file descriptor 3):Bashls -l /proc/$(pgrep secinspect)/fd
Executable & Memory Region Analysis:Verify position-independent execution (PIE), standard libraries, and virtual address mapping:Bashfile secinspect
readelf -h secinspect
ldd secinspect
cat /proc/self/maps
Security Boundary Test:Test access denial when reading a restricted file (secret.txt owned by root with 0600 permissions):Bash./secinspect secret.txt
Kernel returns -1 EACCES (Permission denied), demonstrating that access control is strictly enforced by the OS kernel based on execution identity.Question 2: Linux Permissions, Privilege Management, & Secure ArchitectureKey ObjectivesInvestigate Unix Discretionary Access Control (DAC): Read (4), Write (2), Execute (1).Audit and exploit misconfigured permissions (find . -perm -002).Demonstrate Real User ID (RUID) vs. Effective User ID (EUID) and setuid privilege elevation.Architect a secure, append-only logging environment for audit.log following the Principle of Least Privilege.Lab Setup & User ConfigurationThe lab utilizes isolated test users (testusera, testuserb) and a dedicated system service account (svc-audit):Bash# Create test users
sudo adduser --disabled-password --gecos "" testusera
sudo adduser --disabled-password --gecos "" testuserb

# Create dedicated service account for logging
sudo adduser --system --no-create-home --group svc-audit
Key Demonstrations & Command ReferencePermission Modes & Octal Testing:Bashchmod 644 security-lab/public/notice.txt
chmod 600 security-lab/private/confidential.txt
chmod 700 security-lab/application
Predict-and-test validation proves that non-owners cannot traverse mode 700 directories or read mode 600 files.Auditing & Remediation of World-Writable Weaknesses:Discover:Bashfind . -perm -002
Exploit Verification: Non-owner accounts (testuserb) append unauthorized entries to world-writable (666) files.Remediate: Tighten permissions to 600/640 to restore kernel-level write blocking.Real UID vs. Effective UID (setuid) Demonstration:Compile whoami_ids.c to inspect credential states:Bashgcc -o whoami_ids whoami_ids.c
sudo chown root:root whoami_ids
sudo chmod 4755 whoami_ids
./whoami_ids
Output demonstrates RUID = 1000 (muleya) and EUID = 0 (root), showing how setuid allows temporary controlled privilege escalation.Secure audit.log Engineering Decision:Bashsudo chown svc-audit:svc-audit security-lab/logs/audit.log
sudo chmod 640 security-lab/logs/audit.log
Service Execution: Running under svc-audit grants append capabilities.Unprivileged Users: Accounts like testusera receive Permission denied.Administrator: System administrators inspect and manage the log via standard elevated access.
