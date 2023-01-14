#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "banker.h"

int testTrivialMutualExclusion() {
    return 1;
}

int testTrivialHoldAndWait() {
    return 1;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        puts("Numar incorect de argumente!");
        puts("Utilizare: ./test [fileName] [bankerIterations]");
        return 1;
    }

    char *buf = read_file(argv[1]);
    if (buf == NULL) {
        return 1;
    }

    puts(buf);
    free(buf);
    return 0;
}