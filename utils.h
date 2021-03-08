//
// Created by barab on 06.03.2021.
//

#ifndef UDPAPP_UTILS_H
#define UDPAPP_UTILS_H

#include <stdio.h>
#include <string>

#define DATA "DATA{"
#define NAME "NAME={"
#define SIZE "SIZE={"
#define SHA  "SHA={"
#define NUMBER "N={"
#define START "START"
#define STOP "STOP"

#define SUM "SUM="
#define CRC "CRC={"
#define ACK "ACK"

#define BUFFERS_LEN 1024 - 42



void clear_buffer(char *b, int len);
int get_filesize(FILE *file);
int write_file(char *buf, int s, FILE *file);

#endif //UDPAPP_UTILS_H
