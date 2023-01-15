#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "banker.h"

int testTrivialMutualExclusion(struct Program *p) {
    int fv[100005];
    int N = p->cnt_threads;

    for(int i = 0; i < N; i++)
        fv[i] = -1;

    for(int i = 0; i < N; i++) {
        struct Thread *crt = p->threads[i];
        int szThread = crt->cnt_instructions;

        for(int j = 0; j < szThread; j++) {
            if(fv[crt->instr_res[j]] != -1)
                return 0;
            fv[crt->instr_res[j]] = i;
        }
    }
    return 1;
}

int testTrivialHoldAndWait() {
    int N = p->cnt_threads;

    for(int i = 0; i < N; i++) {
        struct Thread *crt = p->threads[i];
        int szThread = crt->cnt_instructions;

        for(int j = 0; j < szThread; j += 2) {
            int fst_type = crt->instr_type[j], fst_value = crt->instr_res[j];
            int snd_type = crt->instr_type[j + 1], snd_value = crt->instr_res[j + 1];
            
            if(!(fst_type == 1 && snd_type == -1 && fst_value == snd_value))
                return 0;
        }
    }
    return 1;
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

    // free program*

    return 0;
}