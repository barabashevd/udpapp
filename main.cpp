// UDP_Communication_Framework.cpp : Defines the entry point for the console application.
//
/*
 * NAME=file.txt
 * SIZE=1152
 * START
 * DATA{4B pozice v souboru, binárně (uint32_t)}{data 1B až PACKET_MAX_LEN - 4}
 * DATA{4B pozice v souboru, binárně (uint32_t)}{data 1B až PACKET_MAX_LEN - 4}
 * DATA{4B pozice v souboru, binárně (uint32_t)}{data 1B až PACKET_MAX_LEN - 4}
 * ...
 * STOP
 */

/*
 * set timeout in msec for socketS
 * WINDOWS: Timeout value is a DWORD in milliseconds, address passed to setsockopt() is const char *
 * LINUX : Timeout value is a struct timeval, address passed to setsockopt() is const void *
 * DWORD read_timeout = 10000;
 * setsockopt(socketS, SOL_SOCKET, SO_RCVTIMEO,(char*)&read_timeout, sizeof read_timeout);
 */

#pragma comment(lib, "ws2_32.lib")

#include "stdafx.h"
#include <winsock2.h>
#include "ws2tcpip.h"
#include <string>

#include "sender.h"
#include "receiver.h"
#include "utils.h"

#define SENDER_PORT 8888
#define RECEIVER_PORT 5555

#define SENDER_FLAG "-s"
#define IP_FLAG "-ip"
#define FILENAME_FLAG "-f"
#define HELP "--help"

void printf_flags();

int main(int argc, char *argv[]) {
    bool sender = false;
    char *filename = nullptr;
    char *target_ip = nullptr;
    int target_port, local_port;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], HELP) == 0) {
            printf_flags();
            return 1;
        }

        if (strcmp(argv[i], SENDER_FLAG) == 0) {
            printf("App in sender mode.\n");
            sender = true;
            target_port = RECEIVER_PORT;
            local_port = SENDER_PORT;
        }
        if (strcmp(argv[i], IP_FLAG) == 0) {
            target_ip = argv[++i];
            printf("Target IP is: %s\n", target_ip);
        }
        if (strcmp(argv[i], FILENAME_FLAG) == 0) {
            filename = argv[++i];
            printf("File to load: %s\n", filename);
        }
    }

    if (!sender) {
        printf("App in receiver mode.\n");
        target_port = SENDER_PORT;
        local_port = RECEIVER_PORT;
    }

    int result;
    if (sender) {
        result = send_file(target_ip, filename, target_port, local_port);

        if(result != 0){
            printf("Sorry, something went wrong. ERRNO = %i", result);
        }

    } else {
        result = receive_file(target_ip, target_port, local_port);

        if (!result) {
            printf("Sorry, something went wrong. ERRNO = %i", result);
        }
    }

    printf("Sending done...\n");

    return result;
}

void printf_flags(){
    printf("Flags\n");
    printf("\t-s for sender flag\n");
    printf("\t-ip [IP] = set target ip\n");
    printf("\t-f [FILENAME] = set filename to load\n");
    printf("\t--help = shows all flags");
}
