#ifndef PARSER_H
#define PARSER_H

struct Thread {
    int *max_res;

    int *instr_type;
    int *instr_res;
};

struct Program {
    int *available;
    struct Thread *threads;
};

struct Program *read_file(char *file_name);

#endif
