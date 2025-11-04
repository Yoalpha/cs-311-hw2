# CS311 — Assignment 2
## Thread Synchronization and Data Races
Instructor: Melissa Carlson

### Files
- main.c (edit this)
- user_data.c / user_data.h (helpers, do not edit)
- Makefile (builds `thread_sync`)
- CS311_Assignment2_Thread_Synchronization_InstructorCarlson.pdf (handout)

### Build
    make

### Run
    ./thread_sync <nthreads>

Examples:
    ./thread_sync 1
    ./thread_sync 4

### Steps
1) Run with no locks and observe missing users.
2) Add a global mutex protecting all table access.
3) Later, implement finer-grained locking as described in the handout.

### Clean
    make clean
