#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

char *read_file(char *file_name) {
    struct stat file_stat;
    int r = stat(file_name, &file_stat);
    if (r == -1) {
        puts("Fisierul dat nu exista!");
        return NULL;
    }

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

    return buf;
}