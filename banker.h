#ifndef BANKER_H
#define BANKER_H

#include <pthread.h>

struct Banker {
    pthread_mutex_t mu;

    int *available;
    int **max_res;
    int **alocation;
};

struct Banker* init(int no_threads, int *available, int **max_alloc);

int request(struct Banker *banker, int thread, int resource);

void release(struct Banker *banker, int thread, int resource);

#endif
