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


int send_file(char *target_ip, char *filename, int target_port, int local_port);
