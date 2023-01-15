#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "banker.h"

int testTrivialMutualExclusion(struct Program *p) {
    int fv[100005];
    int N = sizeof(p->threads) / sizeof(struct Thread);

    for(int i = 0; i < N; i++)
        fv[i] = -1;

    for(int i = 0; i < N; i++) {
        struct Thread *crt = p->threads[i];
        int szThread = sizeof(crt->instr_res) / sizeof(int);

        for(int j = 0; j < szThread; j++) {
            if(fv[crt->instr_res[j]] != -1)
                return 0;
            fv[crt->instr_res[j]] = i;
        }
    }
    return 1;
}

int testTrivialHoldAndWait() {
    int N = sizeof(p->threads) / sizeof(struct Thread);

    for(int i = 0; i < N; i++) {
        struct Thread *crt = p->threads[i];
        int szThread = sizeof(crt->instr_type) / sizeof(int);

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

    struct Program *p = read_file(argv[1]);
    if(testTrivialMutualExclusion(p) == 1)
        no_deadlock();
    
    if(testTrivialHoldAndWait(p) == 1)
        no_deadlock();

    char *buf = read_file(argv[1]);
    if (buf == NULL) {
        return 1;
    }

    puts(buf);
    free(buf);
    return 0;
}