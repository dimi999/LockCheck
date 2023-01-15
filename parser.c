#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"

#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) > (y)) ? (y) : (x))

struct Program *read_file(char *file_name) {
    struct stat file_stat;
    int r = stat(file_name, &file_stat);
    if (r == -1) {
        puts("Fisierul dat nu exista!");
        return NULL;
    }

    // Read the given file
    int fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        puts("Eroare la deschiderea fisierului!");
        return NULL;
    }

    char *buf = malloc(file_stat.st_size + 1);
    buf[file_stat.st_size] = 0;

    int cnt = read(fd, buf, file_stat.st_size);
    if (cnt != file_stat.st_size) {
        puts("Eroare la citirea fisierului!");
        free(buf);
        close(fd);
        return NULL;
    }

    // Split the file into words
    int cnt_words = 0;
    char word[100][10];
    for (int i = 0; i < 100; i++) {
        word[i][0] = '\0';
    }

    for (int i = 0; i < file_stat.st_size; i++) {
        if (isalnum(buf[i])) {
            strncat(word[cnt_words], &buf[i], 1);
        } else {
            if (strlen(word[cnt_words]) > 0) {
                cnt_words++;
            }
        }

        if (cnt_words >= 100) {
            puts("Fisierul are mai mult de 100 de cuvinte!");
            return NULL;
        }
    }
    if (strlen(word[cnt_words]) > 0) {
        cnt_words++;
    }

    // Find the number of threads, mutexes and semaphores
    int cnt_threads = 0, cnt_mutexes = 0, cnt_semaphores = 0;
    for (int i = 0; i < cnt_words; i++) {
        if (strcmp(word[i], "thread") == 0) {
            cnt_threads++;
        }
        if (strcmp(word[i], "mutex") == 0) {
            cnt_mutexes++;
        }
        if (strcmp(word[i], "semaphore") == 0) {
            cnt_semaphores++;
        }
    }

    // Malloc the necessary arrays for the program
    struct Thread *threads = malloc(cnt_threads * sizeof(struct Thread));
    int *available = malloc((cnt_mutexes + cnt_semaphores) * sizeof(int));

    int current_thread = -1, current_instruction, cnt_instructions;
    for (int i = 0; i < cnt_words; i++) {
        if (strcmp(word[i], "thread") == 0) {
            current_thread++;

            // Count the number of instructions in this thread
            cnt_instructions = 0;
            for (int j = i + 2; j < cnt_words; j += 2) {
                if (strcmp(word[j], "thread") == 0 ||
                    strcmp(word[j], "mutex") == 0 ||
                    strcmp(word[j], "semaphore") == 0) {
                    break;
                } else {
                    cnt_instructions++;
                }
            }

            // Malloc the necessary arrays for the thread
            threads[current_thread].max_resource_allocation = malloc((cnt_mutexes + cnt_semaphores) * sizeof(int));
            int *current_allocation = malloc((cnt_mutexes + cnt_semaphores) * sizeof(int));
            for (int j = 0; j < (cnt_mutexes + cnt_semaphores); j++) {
                current_allocation[j] = 0;
                threads[current_thread].max_resource_allocation[j] = 0;
            }

            threads[current_thread].cnt_instructions = cnt_instructions;
            threads[current_thread].instruction_type = malloc(cnt_instructions * sizeof(int));
            threads[current_thread].instruction_resid = malloc(cnt_instructions * sizeof(int));

            // Parse thread instructions one by one
            current_instruction = -1;
            for (int j = i + 2; j < cnt_words; j += 2) {
                if (strcmp(word[j], "thread") == 0 ||
                    strcmp(word[j], "mutex") == 0 ||
                    strcmp(word[j], "semaphore") == 0) {
                    break;
                }

                current_instruction++;
                int res_ind = atoi(word[j + 1]); 

                if (strcmp(word[j], "lock") == 0) {
                    threads[current_thread].instruction_type[current_instruction] = 1;
                    threads[current_thread].instruction_resid[current_instruction] = res_ind - 1;
                    current_allocation[res_ind - 1]++;
                    threads[current_thread].max_resource_allocation[res_ind - 1] = max(
                        threads[current_thread].max_resource_allocation[res_ind - 1],
                        current_allocation[res_ind - 1]
                    );
                } else if (strcmp(word[j], "unlock") == 0) {
                    threads[current_thread].instruction_type[current_instruction] = -1;
                    threads[current_thread].instruction_resid[current_instruction] = res_ind - 1;
                    current_allocation[res_ind - 1]--;
                } else if (strcmp(word[j], "wait") == 0) {
                    threads[current_thread].instruction_type[current_instruction] = 1;
                    threads[current_thread].instruction_resid[current_instruction] = cnt_mutexes + res_ind - 1;
                    current_allocation[cnt_mutexes + res_ind - 1]++;
                    threads[current_thread].max_resource_allocation[cnt_mutexes + res_ind - 1] = max(
                        threads[current_thread].max_resource_allocation[cnt_mutexes + res_ind - 1],
                        current_allocation[cnt_mutexes + res_ind - 1]
                    );
                } else {
                    threads[current_thread].instruction_type[current_instruction] = -1;
                    threads[current_thread].instruction_resid[current_instruction] = cnt_mutexes + res_ind - 1;
                    current_allocation[cnt_mutexes + res_ind - 1]--;
                }
            }

            free(current_allocation);
        }
    }

    // Parse available resource counts - mutexes have a count of 1, semaphores can have count >1
    int cnt_resources = -1;
    for (int i = 0; i < cnt_words; i++) {
        if (strcmp(word[i], "mutex") == 0) {
            cnt_resources++;
            available[cnt_resources] = 1;
        } else if (strcmp(word[i], "semaphore") == 0) {
            cnt_resources++;
            available[cnt_resources] = atoi(word[i + 2]);
        }
    }

    // Return the program
    struct Program *program = malloc(sizeof(struct Program));
    program->available_resources = available;
    program->threads = threads;

    program->cnt_threads = cnt_threads;
    program->cnt_resources = cnt_resources + 1;

    return program;
}

void destroy_program(struct Program *p) {
    for (int i = 0; i < p->cnt_threads; i++) {
        free(p->threads[i].max_resource_allocation);
        free(p->threads[i].instruction_type);
        free(p->threads[i].instruction_resid);
    }

    free(p->threads);
    free(p->available_resources);

    free(p);
}