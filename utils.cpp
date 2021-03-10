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
    fseek(file, 0L, SEEK_SET);

    return size;
}

int write_file(char *buf, int s, FILE *file) {
    if (strncmp(buf, DATA, sizeof(DATA) - 1) == 0) {
        for (int i = sizeof(DATA) - 1; i < s - 1; i++) {
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

int convert_c_str_to_int(char *c_str_integer) {
    int res = 0;
    std::string cpp_str_tmp = std::string(c_str_integer);
    res = (int) std::strtol(cpp_str_tmp.c_str(), nullptr, 10);
    return res;
}
