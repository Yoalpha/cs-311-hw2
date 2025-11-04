/*
 Assignment 2 — Thread Synchronization and Data Races
 CS311 Operating System Fundamentals

 Uses helper functions from user_data.c.

 Initially: no locks → race conditions will occur.
 Students will first add a global mutex for correctness.
 Later, they will explore finer-grained locking approaches.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "user_data.h"   // brings in User struct + helper functions


static User *table = NULL;
static int nthreads = 1;

// --------------------------------------------------------------------
// TODO (Part 2)G: Add a global mutex lock here for protecting the table.
// Example:
// static pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;
// --------------------------------------------------------------------
//static pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;


// --------------------------------------------------------------------
// TODO (Part 4)S: Add a global mutex lock here for protecting the table.
// Example:
// static pthread_mutex_t seg_locks[NUM_SEGMENTS];
// --------------------------------------------------------------------
static pthread_mutex_t seg_locks[NUM_SEGMENTS];


typedef struct {
    int tid;
    unsigned long long put_time_ns;
    unsigned long long get_time_ns;
    int missing;
} thread_info_t;

// --------------------------------
// TODO (Part 3)B: Add a barrier.
// Example:
// static pthread_barrier_t phase_barrier;
// --------------------------------
// static pthread_barrier_t phase_barrier;


//-------------------------------------------------------------------------------------
// TODO (Part 4)S: - Per-Segment Locks):
// Implement this function as part of the segment-lock section of the assignment.
// Each segment contains 100 users (IDs 0-99 -> segment 0, 100-199 -> segment 1, etc.).
// This function should return which segment a given user ID belongs to.
//-------------------------------------------------------------------------------------
int seg_index_for_id(int id) {
    // TODO (Part 4)S: Compute and return which segment this id writes to A
    int res = id/100;
    return (res); // placeholder - update with correct return value
  }


// Insert a user record into the shared table
void put_user(int id) {
    User u;
    make_name_email(id, u.name, MAX_NAME, u.email, MAX_EMAIL);
    u.id = id;
    u.valid = 1;

    table[id] = u;  // initially unprotected (causes race conditions)
}


// Retrieve a user record by ID
int get_user(int id) {
    int found = 0;

    User u = table[id];
    User e;
    if (u.valid && u.id == id) found = 1;

// For debug: uncomment the next 6 lines if you want to see the actual data vs. expected data    
//    snprintf(e.name, MAX_NAME, "User_%d", id);
//    snprintf(e.email, MAX_EMAIL, "user%d@example.com", id);

//   if (strcmp(u.name,e.name) || (strcmp(u.email,e.email))) {
//        printf("\n INVALID DATA actual: %s %s\n", u.name, u.email);
//        printf("INVALID DATA expecting: %s %s\n", e.name, e.email);
    
  
    return found;
}


// Thread function: perform put() and get() phases
void *thread_main(void *arg) {
    thread_info_t *ti = (thread_info_t*)arg;
    int tid = ti->tid;
    int users_per_thread = NUM_USERS / nthreads;
    int start = tid * users_per_thread;
    int end   = (tid == nthreads - 1) ? NUM_USERS : start + users_per_thread;
   
    // Phase 1: insert users
    unsigned long long t0 = now_ns();
  
    // ---------------------------------------------------------------
    // TODO (Part 2)G: Wrap the write to the shared table with the lock.
    // Example:
    // pthread_mutex_lock(&global_lock);
    // for (int id = start; id < end; id++) {
    //   put_user(id);
    // }
    // pthread_mutex_unlock(&global_lock);
    // ---------------------------------------------------------------

    // ---------------------------------------------------------------
    // TODO (Part 4)S: Segment lock around the write of the shared table.
    // Example:
    // int seg = seg_index_for_id(id);
    // pthread_mutex_lock(&seg_locks[seg]);
    // for (int id = start; id < end; id++) {
    //	  put_user(id);
    // } 
    // pthread_mutex_unlock(&seg_locks[seg]);
    // ---------------------------------------------------------------
   
//pthread_mutex_lock(&global_lock);

    int seg = seg_index_for_id(id);
    pthread_mutex_lock(&seg_locks[seg]);

    for (int id = start; id < end; id++) {
      put_user(id);
    }

    pthread_mutex_unlock(&seg_locks[seg]);


//pthread_mutex_unlock(&global_lock);


    unsigned long long t1 = now_ns();
    // calculate time spent on puts
    ti->put_time_ns = t1 - t0;

    
    // end of put phase
     
    //-----------------------------------------------------------------
    // TODO (Part 3)B: End put phase barrier by calling _wait
    // Wait wakes up to indicate put is done and all gets can commence
    // Example:
    // pthread_barrier_wait(&phase_barrier);
    //-----------------------------------------------------------------
    // pthread_barrier_wait(&phase_barrier);

    // Get Phase:  look up all users - all threads, each looks up one portion
        
    unsigned long long t2 = now_ns();
    int missing = 0;
  
    // ---------------------------------------------------------------
    // TODO (Part 2)G: Lock around the read of the shared table.
    // Example:
    // pthread_mutex_lock(&global_lock);
    // for (int id = start; id < end; id++) {
    // 	    if (!get_user(id)) missing++;
    // }
    // pthread_mutex_unlock(&global_lock);
    // --------------------------------------------------------------- 
//pthread_mutex_lock(&global_lock);

    for (int id = 0; id < NUM_USERS; id++) {
	    if (!get_user(id)) missing++;
    }
//pthread_mutex_unlock(&global_lock);
 
    unsigned long long t3 = now_ns();
    // calculate time spent on gets
    ti->get_time_ns = t3 - t2;
    ti->missing = missing;

    return NULL;
}


void usage(char *prog) {
    fprintf(stderr, "Usage: %s <nthreads>\n", prog);
    exit(1);
}


int main(int argc, char **argv) {
    if (argc < 2) usage(argv[0]);
    nthreads = atoi(argv[1]);
    if (nthreads <= 0) {
        fprintf(stderr, "nthreads must be > 0\n");
        return 1;
    }

    //--------------------------------------------------------
    // TODO (Part 3)B - Initialize the phase barrier
    // Example:
    // pthread_barrier_init(&phase_barrier, NULL, nthreads);
    //---------------------------------------------------------
    //pthread_barrier_init(&phase_barrier, NULL, nthreads);


    //--------------------------------------------------------
    // TODO (Part 4)S - Initialize segment locks
    // Example:
    // for (int i = 0; i < NUM_SEGMENTS; i++) {
    //    pthread_mutex_init(&seg_locks[i], NULL);
    // }
    //--------------------------------------------------------
    for (int i = 0; i < NUM_SEGMENTS; i++) {
        pthread_mutex_init(&seg_locks[i], NULL);
    }


    // Allocate shared table
    table = calloc(NUM_USERS, sizeof(User));
    if (!table) {
        perror("calloc");
        return 1;
    }

    pthread_t *threads = malloc(sizeof(pthread_t) * nthreads);
    thread_info_t *tinfo = malloc(sizeof(thread_info_t) * nthreads);

    unsigned long long start_all = now_ns();

    // Launch threads
    for (int i = 0; i < nthreads; i++) {
        tinfo[i].tid = i;
        if (pthread_create(&threads[i], NULL, thread_main, &tinfo[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // Wait for threads
    for (int i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }

    //---------------------------------------------------------------------------
    // TO DO (Parts 2, 3 & 4)G,B,S : update printf below to indicate global lock, 
    // barrier or segment lock as appropriate
    // --------------------------------------------------------------------------
    printf("\n -------(no locks)-------\n");

    unsigned long long end_all = now_ns();

    // calculate total time spent in threads (from threadcreate to threadjoin)
    printf("\nTotal completion time = %.6f seconds\n\n", (end_all - start_all) / 1e9);

    // Print timing and results
    for (int i = 0; i < nthreads; i++) {
        printf("%d: put time = %.6f s\n", i, tinfo[i].put_time_ns / 1e9);
        printf("%d: lookup time = %.6f s\n", i, tinfo[i].get_time_ns / 1e9);
        printf("%d: %d users missing\n", i, tinfo[i].missing);
    }

    // num entries missing after all threads have completed
    // for sanity check that all entries did get written to table eventually
    int final_missing = 0;
    for (int id = 0; id < NUM_USERS; id++) {
        if (!get_user(id)) final_missing++;
    }
    printf("\nTotal missing in table after threads complete =%d\n\n",final_missing);
     
    free(threads);
    free(tinfo);
    free(table);
    return 0;
}
