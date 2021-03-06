//
// Created by barab on 06.03.2021.
//

#ifndef UDPAPP_UTILS_H
#define UDPAPP_UTILS_H

#include <stdio.h>
#include <string>

#define DATA "DATA="

void clear_buffer(char *b, int len);
int get_filesize(FILE *file);
int write_file(char *buf, int s, FILE *file);

#endif //UDPAPP_UTILS_H
