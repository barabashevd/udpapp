// UDP_Communication_Framework.cpp : Defines the entry point for the console application.
//
/*
 * NAME=file.txt
 * SIZE=1152
 * STRT
 * DATA{4B pozice v souboru, binárně (uint32_t)}{data 1B až PACKET_MAX_LEN - 4}
 * DATA{4B pozice v souboru, binárně (uint32_t)}{data 1B až PACKET_MAX_LEN - 4}
 * DATA{4B pozice v souboru, binárně (uint32_t)}{data 1B až PACKET_MAX_LEN - 4}
 * ...
 * STOP
 */

#pragma comment(lib, "ws2_32.lib")

#include "stdafx.h"
#include <winsock2.h>
#include "ws2tcpip.h"

#define TARGET_IP "127.0.0.1"

#define BUFFERS_LEN 1024
#define NAME "NAME="
#define SIZE "SIZE="


void init_win_socket() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

void clear_buffer(char *b, int len) {
    for (int i = 0; i < len; i++) {
        b[i] = '\0';
    }
}

int send_file(FILE *fp, char *buf, int s) {
    char ch, ch2;
    for (int i = 0; i < s; i++) {
        ch = fgetc(fp);
        buf[i] = ch;
        if (ch == EOF)
            return 1;
    }
    return 0;
}

int recieve_file(char *buf, int s, FILE *file) {
    int i;
    char ch;
    for (i = 0; i < s; i++) {
        ch = buf[i];
        if (ch == EOF) {
            fwrite(&ch, sizeof(char), 1, file);
            return 1;
        } else {
            fwrite(&ch, sizeof(char), 1, file);
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    bool sender = argc > 1 && strcmp(argv[1], "-s") == 0;
    char *filename = argc > 2 ? argv[2] : nullptr;
    int target_port, local_port;

    if (sender) {
        printf("App in sender mode.\n");
        target_port = 5555;
        local_port = 8888;
    } else {
        printf("App in receiver mode.\n");
        target_port = 8888;
        local_port = 5555;
    }

    SOCKET socketS;

    init_win_socket();

    struct sockaddr_in local{};
    struct sockaddr_in from{};

    int from_len = sizeof(from);
    local.sin_family = AF_INET;
    local.sin_port = htons(local_port);
    local.sin_addr.s_addr = INADDR_ANY;

    socketS = socket(AF_INET, SOCK_DGRAM, 0);

    if (bind(socketS, (sockaddr *) &local, sizeof(local)) != 0) {
        printf("Binding error!\n");
        getchar(); //wait for press Enter
        return 1;
    }


    if (sender) {
        char buffer_tx[BUFFERS_LEN];
        clear_buffer(buffer_tx, BUFFERS_LEN);
        FILE *file = fopen(filename, "r");

        sockaddr_in addrDest{};
        addrDest.sin_family = AF_INET;
        addrDest.sin_port = htons(target_port);
        InetPton(AF_INET, _T(TARGET_IP), &addrDest.sin_addr.s_addr);

        strncpy(buffer_tx, filename, BUFFERS_LEN);
        sendto(socketS, buffer_tx, strlen(buffer_tx), 0, (sockaddr *) &addrDest, sizeof(addrDest));
        printf("Filename sent.\n");

        while (true) {

            if (send_file(file, buffer_tx, BUFFERS_LEN)) {
                sendto(socketS, buffer_tx, strlen(buffer_tx),
                       0,
                       (struct sockaddr *) &addrDest, sizeof(addrDest));
                break;
            }

            sendto(socketS, buffer_tx, strlen(buffer_tx),
                   0,
                   (struct sockaddr *) &addrDest, sizeof(addrDest));
            clear_buffer(buffer_tx, BUFFERS_LEN);
        }
        if (file != nullptr)
            fclose(file);

        closesocket(socketS);
    } else {
        char buffer_rx[BUFFERS_LEN];
        clear_buffer(buffer_rx, BUFFERS_LEN);

        printf("Waiting for filename...\n");

        int wait = recvfrom(socketS, buffer_rx, sizeof(buffer_rx),
                            0, (struct sockaddr *) &from,
                            &from_len);

        FILE *output;

        if (wait != SOCKET_ERROR) {
            output = fopen("output.txt", "w");
        } else {
            printf("Socket error.\n");
            return 1;
        }


        while (true) {
            clear_buffer(buffer_rx, BUFFERS_LEN);
            int rec = recvfrom(socketS, buffer_rx, sizeof(buffer_rx),
                               0, (struct sockaddr *) &from,
                               &from_len);

            if (recieve_file(buffer_rx, BUFFERS_LEN, output) || rec == SOCKET_ERROR) {
                break;
            }
        }

        fclose(output);
        closesocket(socketS);
    }
    printf("Sending done...\n");
    return 0;
}
