#ifndef BANKER_H
#define BANKER_H

void init(int no_threads, int no_resources, int **max_alloc);

int request(int thread, int resource);

void release(int thread, int resource);

#endif
