#ifndef USER_DATA_H
#define USER_DATA_H

#include <stddef.h>

#define NUM_USERS 10000
#define NUM_SEGMENTS 100
#define MAX_NAME 32
#define MAX_EMAIL 64

typedef struct {
    int id;
    char name[MAX_NAME];
    char email[MAX_EMAIL];
    volatile int valid;
} User;

void make_name_email(int id, char *name, size_t nlen, char *email, size_t elen);
unsigned long long now_ns(void);
int seg_index_for_id(int id);

#endif
