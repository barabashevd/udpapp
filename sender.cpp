//
// Created by barab on 06.03.2021.
//

#include "sender.h"

int send_file(char *target_ip, char *filename, int target_port, int local_port){
    SOCKET socketS;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

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
    closesocket(socketS);
    return 0;
}





