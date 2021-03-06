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

#include "crc.h"

#define SENDER_PORT 8888
#define RECEIVER_PORT 5555

#define BUFFERS_LEN 1024 - 42

#define NAME "NAME="
#define SIZE "SIZE="
#define START "START"
#define DATA "DATA="
#define STOP "STOP"

#define SUM "SUM="
#define CRC "CRC="
#define ACK "ACK"

#define SENDER_FLAG "-s"
#define IP_FLAG "-ip"
#define FILENAME_FLAG "-f"
#define HELP "--help"

void clear_buffer(char *b, int len);

void init_win_socket();

int get_filesize(FILE *file);

int write_file(char *buf, int s, FILE *file);

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

    SOCKET socketS;

    init_win_socket();

    struct sockaddr_in local{};
    struct sockaddr_in from{};

    int from_len = sizeof(from);
    local.sin_family = AF_INET;
    local.sin_port = htons(local_port);
    local.sin_addr.s_addr = INADDR_ANY;

    socketS = socket(AF_INET, SOCK_DGRAM, 0);


    // Binds a socket to a port, fails if the port is in use
    if (bind(socketS, (sockaddr *) &local, sizeof(local)) != 0) {
        printf("Binding error!\n");
        return 1;
    }

    sockaddr_in addrDest{};
    addrDest.sin_family = AF_INET;
    addrDest.sin_port = htons(target_port);
    InetPton(AF_INET, _T(target_ip), &addrDest.sin_addr.s_addr);

    if (sender) {
        char buffer_tx[BUFFERS_LEN];
        char response[100];

        FILE *file = fopen(filename, "rb");
        int file_size = get_filesize(file);


        // Sends filename in "NAME=filename" format
        clear_buffer(buffer_tx, BUFFERS_LEN);
        strcpy(buffer_tx, NAME);
        strcat(buffer_tx, filename);
        sendto(socketS, buffer_tx, strlen(buffer_tx), 0, (sockaddr *) &addrDest, sizeof(addrDest));
        printf("Filename sent.\n");

        // Wait for ACK
        int wait = recvfrom(socketS, response, sizeof(response), 0, (struct sockaddr *) &from, &from_len);
        if (wait != SOCKET_ERROR && strncmp(response, ACK, sizeof(ACK) - 1) == 0){
            printf("ACK for filename received, %s\n", response);
        }

        // Sends filesize in "SIZE=[bytes]" format
        clear_buffer(buffer_tx, BUFFERS_LEN);
        strcpy(buffer_tx, SIZE);
        // Tohle jsou celkem kouzla, nejdriv to prevede v c++ do std::string pomoci <string> a pak do C stringu
        strcat(buffer_tx, std::to_string(file_size).c_str());
        sendto(socketS, buffer_tx, strlen(buffer_tx), 0, (sockaddr *) &addrDest, sizeof(addrDest));
        printf("Filesize sent, it is: %i\n", file_size);

        // Sends start flag
        clear_buffer(buffer_tx, BUFFERS_LEN);
        strcpy(buffer_tx, START);
        sendto(socketS, buffer_tx, strlen(buffer_tx), 0, (sockaddr *) &addrDest, sizeof(addrDest));
        printf("Start flag sent.\n");

        int read;
        while (!feof(file)) {
            clear_buffer(buffer_tx, BUFFERS_LEN);
            strcpy(buffer_tx, DATA);
            for (int i = sizeof(DATA) - 1; i < BUFFERS_LEN; i++) {
                if (!feof(file)) {
                    *(buffer_tx + i) = fgetc(file);
                } else {
                    printf("End of file reached.\n");
                    break;
                }
            }
            // Calculate CRC
            unsigned char crc = get_crc(buffer_tx, BUFFERS_LEN, 0xffff, 0);
            printf("CRC: %i\n", crc);

            sendto(socketS, buffer_tx, BUFFERS_LEN, 0, (struct sockaddr *) &addrDest, sizeof(addrDest));
        }

        // Sends stop flag
        clear_buffer(buffer_tx, BUFFERS_LEN);
        strcpy(buffer_tx, STOP);
        sendto(socketS, buffer_tx, strlen(buffer_tx), 0, (sockaddr *) &addrDest, sizeof(addrDest));
        printf("Stop flag sent.\n");

        if (file != nullptr) {
            fclose(file);
        }

    } else {
        char buffer_rx[BUFFERS_LEN];


        FILE *output;

        clear_buffer(buffer_rx, BUFFERS_LEN);
        printf("Waiting for filename...\n");

        int wait = recvfrom(socketS, buffer_rx, sizeof(buffer_rx), 0, (struct sockaddr *) &from, &from_len);

        if (wait != SOCKET_ERROR && strncmp(buffer_rx, NAME, sizeof(NAME) - 1) == 0) {
            char *prefix = strtok(buffer_rx, "=");
            const char *fname = strtok(NULL, "=");
            output = fopen(fname, "wb");
            printf("Filename: %s\n", fname);
        } else {
            printf("Socket error.\n");
            return 1;
        }

        // Sends ACK for filename
        sendto(socketS, ACK, strlen(ACK), 0, (sockaddr *) &addrDest, sizeof(addrDest));

        // -------------------------------------------------------------
        clear_buffer(buffer_rx, BUFFERS_LEN);
        printf("Waiting for file size...\n");

        int rec_size = recvfrom(socketS, buffer_rx, sizeof(buffer_rx),
                                0, (struct sockaddr *) &from,
                                &from_len);


        int integer_fsize;
        if (rec_size != SOCKET_ERROR && strncmp(buffer_rx, SIZE, sizeof(SIZE) - 1) == 0) {
            char *prefix = strtok(buffer_rx, "=");
            const char *fsize = strtok(nullptr, "=");

            std::string str_fsize = std::string(fsize);
            integer_fsize = (int) std::strtol(str_fsize.c_str(), nullptr, 10);
            if (integer_fsize == 0) {
                printf("Error: cannot convert size!\n");
            }

            printf("File size: %d\n", integer_fsize);
        } else {
            printf("Socket error.\n");
            return 1;
        }
        // -----------------------------------------------------------------
        clear_buffer(buffer_rx, BUFFERS_LEN);
        printf("Waiting for start flag...\n");

        int rec_start = recvfrom(socketS, buffer_rx, sizeof(buffer_rx),
                                 0, (struct sockaddr *) &from,
                                 &from_len);

        if (rec_start != SOCKET_ERROR && strncmp(buffer_rx, START, sizeof(START) - 1) == 0) {
            printf("Received flag: %s\n", buffer_rx);
            printf("Start flag received - ready for data\n");
        } else {
            printf("Socket error.\n");
            return 1;
        }

        // -----------------------------------------------------------------
        while (true) {
            clear_buffer(buffer_rx, BUFFERS_LEN);
            int rec = recvfrom(socketS, buffer_rx, sizeof(buffer_rx),
                               0, (struct sockaddr *) &from,
                               &from_len);

            if (rec == SOCKET_ERROR) {
                printf("Socket error!\n");
                break;
            }

            if (strncmp(buffer_rx, STOP, sizeof(STOP) - 1) == 0) {
                printf("Stop flag received!\n");
                break;
            }

            int packet_size = integer_fsize > BUFFERS_LEN ? BUFFERS_LEN : integer_fsize;
            write_file(buffer_rx, packet_size, output);
            integer_fsize = integer_fsize - packet_size + 5;
        }
        fclose(output);
    }
    closesocket(socketS);
    printf("Sending done...\n");

    return 0;
}

void clear_buffer(char *b, int len) {
    for (int i = 0; i < len; i++) {
        b[i] = '\0';
    }
}

void init_win_socket() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

int get_filesize(FILE *file) {
    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return size;
}

int write_file(char *buf, int s, FILE *file) {
    if (strncmp(buf, DATA, sizeof(DATA) - 1) == 0) {
        for (int i = sizeof(DATA) - 1; i < s; i++) {
            // fwrite(buf + i, sizeof(char), 1, file);
            fputc(*(buf + i), file);
        }
    } else {
        printf("Error: invalid structure!\n");
    }
    return 0;
}

void printf_flags(){
    printf("Flags\n");
    printf("\t-s for sender flag\n");
    printf("\t-ip [IP] = set target ip\n");
    printf("\t-f [FILENAME] = set filename to load\n");
    printf("\t--help = shows all flags");
}
