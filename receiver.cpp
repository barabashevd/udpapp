//
// Created by AdamC on 07.03.2021.
//

#include "receiver.h"

int receive_file(char *target_ip, int target_port, int local_port) {
    // TODO function to open socket
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

    char buffer_rx[BUFFERS_LEN];
    //-----------------------------------------------------------


    FILE *output;
    int position = 0;

    clear_buffer(buffer_rx, BUFFERS_LEN);
    printf("Waiting for init info...\n");
    int rec_init_info = recvfrom(socketS, buffer_rx, sizeof(buffer_rx), 0, (struct sockaddr *) &from, &from_len);

    if (rec_init_info == SOCKET_ERROR) {
        fprintf(stderr, "Socket error!");
        return 1;
    }

    // Receives file name

    char *rest;
    if(strncmp(buffer_rx, NAME, sizeof(NAME) - 1) == 0) {
        const char *fname = strtok(buffer_rx, "{");
        fname = strtok(nullptr, "}");
        rest = strtok(nullptr, "");

        output = fopen("output.txt", "wb");
        printf("File name: %s\n", fname);
    }

    // Sends ACK for filename
    //sendto(socketS, ACK, strlen(ACK), 0, (sockaddr *) &addrDest, sizeof(addrDest));

    // Receives file size
    int integer_fsize = 0;
    if (strncmp(rest, SIZE, sizeof(SIZE) - 1) == 0) {
        const char *fsize = strtok(rest, "{");
        fsize = strtok(nullptr, "}");
        rest = strtok(nullptr, "");

        // Convert string to int
        std::string str_fsize = std::string(fsize);
        integer_fsize = (int) std::strtol(str_fsize.c_str(), nullptr, 10);

        printf("File size: %d\n", integer_fsize);

       if (integer_fsize == 0) {
           fprintf(stderr, "Error: cannot convert size!\n");
           return 2;
       }

    }

    // -------------------------------------------------------------
    // Receives sha
    if(strncmp(rest, SHA, sizeof(SHA) - 1) == 0) {
        const char *sha = strtok(rest, "{");
        sha = strtok(nullptr, "}");
        rest = strtok(nullptr, "");

        printf("SHA: %s\n", sha);

    }

    // Receives crc
    if(strncmp(rest, CRC, sizeof(CRC) - 1) == 0) {
        const char *crc = strtok(rest, "{");
        crc = strtok(nullptr, "}");
        rest = strtok(nullptr, "");

        int integer_crc;
        std::string str_crc = std::string(crc);
        integer_crc = (int) std::strtol(str_crc.c_str(), nullptr, 10);
        printf("CRC: %d\n", integer_crc);
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
        fprintf(stderr, "Socket error.\n");
        return 1;
    }

    // -----------------------------------------------------------------
    while (true) {
        clear_buffer(buffer_rx, BUFFERS_LEN);
        int rec = recvfrom(socketS, buffer_rx, sizeof(buffer_rx),
                           0, (struct sockaddr *) &from,
                           &from_len);
        //printf("BUFF: %s\n", buffer_rx);

        if (rec == SOCKET_ERROR) {
            fprintf(stderr, "Socket error.\n");
            return 1;
        }

        if (strncmp(buffer_rx, STOP, sizeof(STOP) - 1) == 0) {
            printf("Stop flag received!\n");
            break;
        }

        int packet_size = integer_fsize > BUFFERS_LEN ? BUFFERS_LEN : integer_fsize;
        write_file(buffer_rx, packet_size - CRC_LEN, output);
        integer_fsize -= (packet_size - sizeof(DATA) - CRC_LEN - 1);
    }
    fclose(output);
    closesocket(socketS);

    return 0;
}
