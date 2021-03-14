//
// Created by barab on 06.03.2021.
//

#include "sender.h"

void add_to_buffer(char *buf, char *name, char *to_write);

int send_data_and_wait(char *buf, int buf_len, char *flag);

SOCKET socketS;
sockaddr_in addrDest{};

char buffer_tx[BUFFERS_LEN];
char response[100];

struct sockaddr_in local{};
struct sockaddr_in from{};
int from_len = sizeof(from);

int send_file(char *target_ip, char *filename, int target_port, int local_port) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    local.sin_family = AF_INET;
    local.sin_port = htons(local_port);
    local.sin_addr.s_addr = INADDR_ANY;

    socketS = socket(AF_INET, SOCK_DGRAM, 0);

    DWORD read_timeout = 10000;
    setsockopt(socketS, SOL_SOCKET, SO_RCVTIMEO, (char *) &read_timeout, sizeof read_timeout);

    // Binds a socket to a port, fails if the port is in use
    if (bind(socketS, (sockaddr *) &local, sizeof(local)) != 0) {
        printf("Binding error!\n");
        return 1;
    }


    addrDest.sin_family = AF_INET;
    addrDest.sin_port = htons(target_port);
    InetPton(AF_INET, _T(target_ip), &addrDest.sin_addr.s_addr);


    FILE *file = fopen(filename, "rb");
    int file_size = get_filesize(file);

    MD5 md5;
    char *hashed = md5.digestFile(filename);
    printf("Hashed file: %s\n", hashed);


    // Sends filename in NAME={filename}SIZE={size}HASH={sha}CRC={crc}
    clear_buffer(buffer_tx, BUFFERS_LEN);

    add_to_buffer(buffer_tx, NAME, filename);
    add_to_buffer(buffer_tx, SIZE, (char *) std::to_string(file_size).c_str());
    add_to_buffer(buffer_tx, HASH, hashed);

    int init_crc = get_crc(buffer_tx, strlen(buffer_tx), 0xffff, 0);
    add_to_buffer(buffer_tx, CRC, (char *) std::to_string(init_crc).c_str());
    send_data_and_wait(buffer_tx, strlen(buffer_tx), "INIT");
    send_data_and_wait(START, strlen(START), START);


    int read;
    int counter = 0;
    int crc_size = CRC_LEN + 1;

    // Data are sent in format: DATA{data...}NUMBER={n}CRC={crc}
    while (!feof(file)) {
        clear_buffer(buffer_tx, BUFFERS_LEN);
        strcpy(buffer_tx, DATA);
        int chars_read = 0;
        int pos;

        for (pos = sizeof(DATA) - 1; pos < BUFFERS_LEN - crc_size; pos++) {
            if (!feof(file)) {
                *(buffer_tx + pos) = fgetc(file);
                chars_read += 1;
            } else {
                printf("End of file reached.\n");
                //pos--;
                break;
            }
        }

        char packet_tail[CRC_LEN + 1];
        // Add packet number
        strcpy(packet_tail, "}");
        add_to_buffer(packet_tail, NUMBER, (char *) std::to_string(counter).c_str());

        int tail_len = strlen(packet_tail);
        counter++;

        for (int i = 0; i < tail_len; i++) {
            *(buffer_tx + pos + i) = *(packet_tail + i);
        }

        pos += tail_len;

        // Calculate CRC
        clear_buffer(packet_tail, CRC_LEN);
        int data_crc = get_crc(buffer_tx, chars_read + sizeof(DATA), 0xffff, 0);
        add_to_buffer(packet_tail, CRC, (char *) std::to_string(data_crc).c_str());

        tail_len = strlen(packet_tail);
        for (int i = 0; i < tail_len; i++) {
            *(buffer_tx + pos + i) = *(packet_tail + i);
        }

        send_data_and_wait(buffer_tx, BUFFERS_LEN, NULL);
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

void add_to_buffer(char *buf, char *name, char *to_write) {
    strcat(buf, name);
    strcat(buf, to_write);
    strcat(buf, "}");
}

int send_data_and_wait(char *buf, int buf_len, char *flag) {
    sendto(socketS, buf, buf_len, 0, (sockaddr *) &addrDest, sizeof(addrDest));
    printf("Packet sent. %s\n", flag);

    int wait = recvfrom(socketS, response, sizeof(response), 0, (struct sockaddr *) &from, &from_len);
    while (!(wait != SOCKET_ERROR && strncmp(response, ACK, sizeof(ACK) - 1) == 0)) {
        if (strncmp(response, NOT_ACK, sizeof(NOT_ACK) - 1) == 0) {
            printf("Received NOT ACK, resending packet.\n");
        } else {
            printf("ACK not received or dataloss occurred. Resending packet..\n");
        }
        sendto(socketS, buf, buf_len, 0, (sockaddr *) &addrDest, sizeof(addrDest));
        wait = recvfrom(socketS, response, sizeof(response), 0, (struct sockaddr *) &from, &from_len);
    }
    if (flag != NULL) {
        printf("ACK %s received.\n", flag);
    }
    return 1;
}



