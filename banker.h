#ifndef BANKER_H
#define BANKER_H

#include <pthread.h>
#include "parser.h"

struct Banker {
    pthread_mutex_t mtx;
    int cnt_resources, cnt_threads;

    int *available_resources;
    int **max_allocation;
    int **allocation;
};

struct Banker* init(struct Program *p);

int request(struct Banker *banker, int thread, int resource);

void release(struct Banker *banker, int thread, int resource);

void destroy_banker(struct Banker *banker);

#endif
