#ifndef PARSER_H
#define PARSER_H

struct Thread {
    int *max_resource_allocation; // max quantity of each resource that the thread will need

    int cnt_instructions;
    int *instruction_type; // +1 for request, -1 for release
    int *instruction_resid; // id of the requested/released resource
};

struct Program {
    int cnt_threads, cnt_resources;

    int *available_resources;
    struct Thread *threads;
};

struct Program *read_file(char *file_name);

void destroy_program(struct Program *p);

#endif
