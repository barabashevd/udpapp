//
// Created by barab on 06.03.2021.
//

#include "sender.h"
#include "md5.h"

void add_to_buffer(char *buf, char *name, char *to_write);

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
    MD5 md5;
    char *hashed = md5.digestFile(filename);
    printf("Hashed file: %s\n", hashed);


    // Sends filename in NAME={filename}SIZE={size}HASH={sha}CRC={crc}
    clear_buffer(buffer_tx, BUFFERS_LEN);

    int init_crc = get_crc(buffer_tx, strlen(buffer_tx), 0xffff, 0);

    add_to_buffer(buffer_tx, NAME, filename);
    /*
    add_to_buffer(buffer_tx, SIZE, (char *) std::to_string(file_size).c_str());
    add_to_buffer(buffer_tx, HASH, hashed);
    add_to_buffer(buffer_tx, CRC, (char *) std::to_string(init_crc).c_str());
    */

    sendto(socketS, buffer_tx, strlen(buffer_tx), 0, (sockaddr *) &addrDest, sizeof(addrDest));
    printf("Init info sent.\n");

    // TODO: add timeout
    /*
    int wait = recvfrom(socketS, response, sizeof(response), 0, (struct sockaddr *) &from, &from_len);
    if (wait != SOCKET_ERROR && strncmp(response, ACK, sizeof(ACK) - 1) == 0){
        printf("ACK for filename received, %s\n", response);
    }
    */

    // Sends start flag
    clear_buffer(buffer_tx, BUFFERS_LEN);
    strcpy(buffer_tx, START);
    sendto(socketS, buffer_tx, strlen(buffer_tx), 0, (sockaddr *) &addrDest, sizeof(addrDest));
    printf("Start flag sent.\n");

    // Wait for ACK for start flag TODO

    int read;
    int counter = 0;
    int crc_size = CRC_LEN + 1;
    // Data are sent in format: DATA{data...}NUMBER={n}CRC={crc}

    while (!feof(file)) {
        clear_buffer(buffer_tx, BUFFERS_LEN);
        strcpy(buffer_tx, DATA);
        int chars_read = 0;
        int pos;

        for (pos = sizeof(DATA) - 1; pos < BUFFERS_LEN - crc_size ; pos++) {
            if (!feof(file)) {
                *(buffer_tx + pos) = fgetc(file);
                chars_read += 1;
            } else {
                printf("End of file reached.\n");
                break;
            }
        }

        char pakcet_tail[CRC_LEN + 1];
        // Add packet number
        strcpy(pakcet_tail, "}");
        strcat(pakcet_tail, NUMBER);
        strcat(pakcet_tail, std::to_string(counter).c_str());
        strcat(pakcet_tail, "}");
        int tail_len = strlen(pakcet_tail);
        counter++;

        for (int i = 0; i < tail_len; i++){
            *(buffer_tx + pos + i) = *(pakcet_tail + i);
        }

        pos += tail_len;

        // Calculate CRC
        clear_buffer(pakcet_tail, CRC_LEN);
        int data_crc = get_crc(buffer_tx, chars_read + sizeof(DATA), 0xffff, 0);
        add_to_buffer(pakcet_tail, CRC, (char *)  std::to_string(data_crc).c_str());
        tail_len = strlen(pakcet_tail);

        for (int i = 0; i < tail_len; i++){
            *(buffer_tx + pos + i) = *(pakcet_tail + i);
        }

        sendto(socketS, buffer_tx, BUFFERS_LEN, 0, (struct sockaddr *) &addrDest, sizeof(addrDest));
    }

    // Sends stop flag
    clear_buffer(buffer_tx, BUFFERS_LEN);
    strcpy(buffer_tx, STOP);
    sendto(socketS, buffer_tx, strlen(buffer_tx), 0, (sockaddr *) &addrDest, sizeof(addrDest));
    printf("Stop flag sent.\n");

    // TODO: wait for ACK for STOP flag

    if (file != nullptr) {
        fclose(file);
    }
    closesocket(socketS);
    return 0;
}

void add_to_buffer(char *buf, char *name, char *to_write){
    strcpy(buf, name);
    strcat(buf, to_write);
    strcat(buf, "}");
}





