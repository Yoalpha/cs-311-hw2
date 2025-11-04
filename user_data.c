#include "user_data.h"
#include <stdio.h>
#include <sys/time.h>

void make_name_email(int id, char *name, size_t nlen, char *email, size_t elen) {
    snprintf(name, nlen, "User_%d", id);
    snprintf(email, elen, "user%d@example.com", id);
}

unsigned long long now_ns(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (unsigned long long)tv.tv_sec * 1000000000ULL +
           (unsigned long long)tv.tv_usec * 1000ULL;
}

