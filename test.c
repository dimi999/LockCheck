#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "banker.h"

int testTrivialMutualExclusion(struct Program* p) {
    return 1;
}

int testTrivialHoldAndWait(struct Program* p) {
    return 1;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        puts("Numar incorect de argumente!");
        puts("Utilizare: ./test [fileName] [bankerIterations]");
        return 1;
    }

    struct Program *p = read_file(argv[1]);
    if (p == NULL) {
        return 1;
    }

    printf("Num threads: %d\n", p->cnt_threads);
    printf("Num res: %d\n", p->cnt_resources);

    return 0;
}