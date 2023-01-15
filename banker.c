#include "banker.h"
#include <stdlib.h>

struct Banker* init(int no_threads, int *available, int **max_alloc) {
    struct Banker* res = malloc(sizeof(res));
    pthread_mutex_init(&res->mu, NULL);
    res->available = available;
    res->max_res = max_alloc;
    res->allocation = malloc(sizeof(res) * no_threads);
    int M = sizeof(available) / sizeof(int);

    for(int i = 0; i < no_threads; i++) 
        res->allocation[i] = malloc(sizeof(int) * M);
    
    for(int i = 0; i < no_threads; i++)
        for(int j = 0; j < M; j++)
            res->allocation[i][j] = 0;

    return res;
}

int request(struct Banker *banker, int thread, int resource) {
    
    return 0;
}

void release(struct Banker *banker, int thread, int resource) {

}