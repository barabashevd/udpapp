//
// Created by barab on 06.03.2021.
//

#include "utils.h"

void clear_buffer(char *b, int len);
int get_filesize(FILE *file);
int write_file(char *buf, int packet_size, FILE *file);

int get_filesize(FILE *file) {
    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    printf("Size in func: %d \n", size);
    fseek(file, 0L, SEEK_SET);

    return size;
}

int write_file(char *buf, int s, FILE *file) {
    if (strncmp(buf, DATA, sizeof(DATA) - 1) == 0) {
        for (int i = sizeof(DATA) - 1; i < s; i++) {
            fputc(*(buf + i), file);
        }
    } else {
        printf("Error: invalid structure!\n");
    }
    return 0;
}

void clear_buffer(char *b, int len) {
    for (int i = 0; i < len; i++) {
        b[i] = '\0';
    }
}
