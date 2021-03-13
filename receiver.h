//
// Created by AdamC on 07.03.2021.
//



#include <stdio.h>
#include <string>
#include <winsock2.h>
#include "ws2tcpip.h"
#include <tchar.h>

#include "crc.h"
#include "utils.h"


int receive_file(char *sender_ip, int sender_port, int local_port);

int strip_data(char **buff_ptr, char *tag, char **data);
