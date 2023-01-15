#include "banker.h"
#include <stdlib.h>

// create initial banker state from a program
struct Banker* init(struct Program *p) {
    struct Banker* banker = malloc(sizeof(struct Banker));

    pthread_mutex_init(&banker->mtx, NULL);
    banker->cnt_resources = p->cnt_resources;
    banker->cnt_threads = p->cnt_threads;

    banker->available_resources = malloc(banker->cnt_resources * sizeof(int));
    for (int i = 0; i < banker->cnt_resources; i++) {
        banker->available_resources[i] = p->available_resources[i];
    }

    banker->max_allocation = malloc(banker->cnt_threads * sizeof(int *));
    for (int i = 0; i < banker->cnt_threads; i++) {
        banker->max_allocation[i] = malloc(banker->cnt_resources * sizeof(int));
        for (int j = 0; j < banker->cnt_resources; j++) {
            banker->max_allocation[i][j] = p->threads[i].max_resource_allocation[j];
        }
    }

    banker->allocation = malloc(banker->cnt_threads * sizeof(int *));
    for (int i = 0; i < banker->cnt_threads; i++) {
        banker->allocation[i] = malloc(banker->cnt_resources * sizeof(int));
        for (int j = 0; j < banker->cnt_resources; j++) {
            banker->allocation[i][j] = 0;
        }
    }

    return banker;
}

int request(struct Banker *banker, int thread, int resource) {
    pthread_mutex_lock(&banker->mtx);

    // insufficient resources to grant request
    if (banker->available_resources[resource] < 1) {
        pthread_mutex_unlock(&banker->mtx);
        return 1;
    }

    // we have enough resources to grant the request
    // check if granting the request transitions to a safe state
    banker->available_resources[resource]--;
    banker->allocation[thread][resource]++;

    int *finished = malloc(banker->cnt_threads * sizeof(int));
    for (int i = 0; i < banker->cnt_threads; i++) {
        finished[i] = 0;
    }

    for (int i = 0; i < banker->cnt_threads; i++) {
        for (int j = 0; j < banker->cnt_threads; j++) {
            if (finished[j]) { continue; }

            int can_finish = 1;
            for (int r = 0; r < banker->cnt_resources; r++) {
                int need = banker->max_allocation[j][r] - banker->allocation[j][r];
                if (need > banker->available_resources[r]) {
                    can_finish = -1;
                    break;
                }
            }

            if (can_finish == 1) {
                finished[j] = 1;
                for (int r = 0; r < banker->cnt_resources; r++) {
                    banker->available_resources[r] += banker->allocation[j][r];
                }
            }
        }
    }

    int cnt_finished = 0;
    for (int i = 0; i < banker->cnt_threads; i++) {
        if (finished[i]) {
            cnt_finished++;
            for (int r = 0; r < banker->cnt_resources; r++) {
                banker->available_resources[r] -= banker->allocation[i][r];
            }
        }
    }
    banker->allocation[thread][resource]--;
    banker->available_resources[resource]++;

    // in this case, the state is unsafe
    if (cnt_finished != banker->cnt_threads) {
        pthread_mutex_unlock(&banker->mtx);
        return 2;
    }

    // grant request
    banker->available_resources[resource]--;
    banker->allocation[thread][resource]++;
    pthread_mutex_unlock(&banker->mtx);    
    return 0;
}

void release(struct Banker *banker, int thread, int resource) {
    pthread_mutex_lock(&banker->mtx);
    banker->available_resources[resource]++;
    banker->allocation[thread][resource]--;
    pthread_mutex_unlock(&banker->mtx);
}

void destroy_banker(struct Banker *banker) {
    for (int i = 0; i < banker->cnt_threads; i++) {
        free(banker->max_allocation[i]);
        free(banker->allocation[i]);
    }

    free(banker->available_resources);
    free(banker->max_allocation);
    free(banker->allocation);

    pthread_mutex_destroy(&banker->mtx);

    free(banker);
}