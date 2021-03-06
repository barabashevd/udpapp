//
// Created by barab on 06.03.2021.
//

#include <stdio.h>
#include <string>
#include <winsock2.h>
#include "ws2tcpip.h"
#include <tchar.h>

#include "crc.h"
#include "utils.h"

#define NAME "NAME="
#define SIZE "SIZE="
#define START "START"
#define DATA "DATA="
#define STOP "STOP"

#define SUM "SUM="
#define CRC "CRC="
#define ACK "ACK"

#define BUFFERS_LEN 1024 - 42

int send_file(char *target_ip, char *filename, int target_port, int local_port);
