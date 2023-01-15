#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "parser.h"
#include "banker.h"

int CNT_STARVATION = 10;
int DEBUG = 0;

int testTrivialMutualExclusion(struct Program* p) {
    int fv[100];
    for (int i = 0; i < p->cnt_threads; i++) {
        fv[i] = -1;
    }

    for (int i = 0; i < p->cnt_threads; i++) {
        struct Thread crt = p->threads[i];

        for (int j = 0; j < crt.cnt_instructions; j++) {
            if (fv[crt.instruction_resid[j]] != -1 && fv[crt.instruction_resid[j]] != i) {
                return 0;
            }
            fv[crt.instruction_resid[j]] = i;
        }
    }
    
    return 1;
}

int testTrivialHoldAndWait(struct Program* p) {
    for (int i = 0; i < p->cnt_threads; i++) {
        struct Thread crt = p->threads[i];

        for (int j = 0; j < crt.cnt_instructions; j += 2) {
            int fst_type = crt.instruction_type[j], fst_value = crt.instruction_resid[j];
            int snd_type = crt.instruction_type[j + 1], snd_value = crt.instruction_resid[j + 1];
            
            if (!(fst_type == 1 && snd_type == -1 && fst_value == snd_value)) {
                return 0;
            }
        }
    }

    return 1;
}

void debug(struct Program *program) {
    printf("Cnt threads: %d\n", program->cnt_threads);
    printf("Cnt res: %d\n", program->cnt_resources);

    for (int i = 0; i < program->cnt_resources; i++) {
        printf("Resursa %d avem %d\n", i, program->available_resources[i]);
    }

    puts("=====");

    for (int i = 0; i < program->cnt_threads; i++) {
        printf("Threadul %d are %d instructiuni!\n", i, program->threads[i].cnt_instructions);

        for (int j = 0; j < program->threads[i].cnt_instructions; j++) {
            printf("%d %d\n", program->threads[i].instruction_type[j], 
                            program->threads[i].instruction_resid[j]);
        }

        printf("Max alloc pt threadul %d:\n", i);
        for (int j = 0; j < program->cnt_resources; j++) {
            printf("%d %d\n", j, program->threads[i].max_resource_allocation[j]);
        }
        puts("=====");
    }
}

struct ThreadInfo {
    int id;
    struct Thread *t;
};

pthread_mutex_t mtx;
int deadlocked, starved;
struct Banker *banker;

void *run_thread(void *p) {
    struct ThreadInfo *tinfo = (struct ThreadInfo *)p;

    for (int i = 0; i < tinfo->t->cnt_instructions; i++) {        
        pthread_mutex_lock(&mtx);
        if (deadlocked || starved) {
            pthread_mutex_unlock(&mtx);
            break;
        }
        pthread_mutex_unlock(&mtx);

        if (tinfo->t->instruction_type[i] == 1) {
            int r = rand() % 10 + 1;

            // Sleep random number of milliseconds to simulate work
            usleep(r * 1000);

            int cnt_retry = 0;
            while (cnt_retry < CNT_STARVATION) {
                int r = request(banker, tinfo->id, tinfo->t->instruction_resid[i]);
                if (DEBUG) {
                    printf("%d got %d on instruction %d\n", tinfo->id, r, i);
                }

                if (r == 0) { // request granted
                    break;
                }
                if (r == 1) { // request denied because of lack of available resources; try again later
                    cnt_retry++;
                    r = rand() % 10 + 1;
                    usleep(r * 1000);
                }
                if (r == 2) { // deadlock found
                    pthread_mutex_lock(&mtx);
                    deadlocked = 1;
                    pthread_mutex_unlock(&mtx);
                    break;
                }
            }

            if (cnt_retry >= CNT_STARVATION) {
                pthread_mutex_lock(&mtx);
                starved = 1;
                pthread_mutex_unlock(&mtx);
            }
        } else {
            release(banker, tinfo->id, tinfo->t->instruction_resid[i]);
        }
    }

    return NULL;
}

void run_banker(struct Program *program) {
    pthread_t *threads = malloc(program->cnt_threads * sizeof(pthread_t));
    struct ThreadInfo *tinfo = malloc(program->cnt_threads * sizeof(struct ThreadInfo));
    for (int i = 0; i < program->cnt_threads; i++) {
        tinfo[i].id = i;
        tinfo[i].t = &(program->threads[i]);
    }

    for (int i = 0; i < program->cnt_threads; i++) {
        pthread_create(&threads[i], NULL, run_thread, &tinfo[i]);
    }

    for (int i = 0; i < program->cnt_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    free(tinfo);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        puts("Numar incorect de argumente!");
        puts("Utilizare: ./test [fileName] [bankerIterations]");
        return 1;
    }

    struct Program *program = read_file(argv[1]);
    if (program == NULL) {
        return 1;
    }

    if (DEBUG) {
        debug(program);
    }

    if (testTrivialMutualExclusion(program)) {
        puts("Programul nu poate avea deadlock sau starvation, a trecut check-ul Mutual Exclusion");
        destroy_program(program);
        return 0;
    }

    if (testTrivialHoldAndWait(program)) {
        puts("Programul nu poate avea deadlock sau starvation, a trecut check-ul Hold And Wait");
        destroy_program(program);
        return 0;
    }

    srand(time(NULL));
    pthread_mutex_init(&mtx, NULL);

    int iters = atoi(argv[2]);
    int cnt_ok = 0, cnt_deadlock = 0, cnt_starvation = 0;
    for (int i = 0; i < iters; i++) {
        banker = init(program);
        deadlocked = 0;
        starved = 0;

        run_banker(program);
        if (deadlocked) {
            cnt_deadlock++;
        } else if (starved) {
            cnt_starvation++;
        } else {
            cnt_ok++;
        }

        destroy_banker(banker);

        if (DEBUG) {
            char *res = "OK";
            if (deadlocked) {
                res = "Deadlock";
            } else if (starved) {
                res = "Starved";
            }
            printf("Run %d\n got %s\n", i + 1, res);
        }
    }

    printf("Ran %d times. Got:\n", iters);
    printf("%d ok\n", cnt_ok);
    printf("%d deadlocks\n", cnt_deadlock);
    printf("%d starvations\n", cnt_starvation);

    pthread_mutex_destroy(&mtx);

    destroy_program(program);

    return 0;
}