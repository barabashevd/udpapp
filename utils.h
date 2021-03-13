//
// Created by barab on 06.03.2021.
//

#ifndef UDPAPP_UTILS_H
#define UDPAPP_UTILS_H

#include <stdio.h>
#include <string>
#include <winsock2.h>
#include "md5.h"

#define DATA "DATA{"
#define NAME "NAME={"
#define SIZE "SIZE={"
#define HASH  "MD5={"
#define NUMBER "N={"
#define START "START"
#define STOP "STOP"

#define SUM "SUM="
#define CRC "CRC={"
#define ACK "ACK"
#define NOT_ACK "NACK"
#define CRC_LEN 50

#define BUFFERS_LEN 982
#define CRC_LEN 50


void clear_buffer(char *b, int len);

int get_filesize(FILE *file);

int write_file(char *buf, int packet_size, FILE *file);

int convert_c_str_to_int(char *str_integer);


#endif //UDPAPP_UTILS_H
