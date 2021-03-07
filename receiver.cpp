//
// Created by AdamC on 07.03.2021.
//

#include "receiver.h"

int receive_file(char *target_ip, int target_port, int local_port) {
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
    closesocket(socketS);

    return 0;
}
