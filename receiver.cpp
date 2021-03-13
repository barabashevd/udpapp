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
    // InetPton(AF_INET, _T(target_ip), &addrDest.sin_addr.s_addr);

    char buffer_rx[BUFFERS_LEN];

    //-----------------------------------------------------------
    clear_buffer(buffer_rx, BUFFERS_LEN);
    printf("Waiting for init info...\n");
    int rec_init_info = recvfrom(socketS, buffer_rx, sizeof(buffer_rx), 0, (struct sockaddr *) &from, &from_len);

    addrDest.sin_addr.s_addr = from.sin_addr.S_un.S_addr;

    //sendto(socketS, "test", strlen("test"), 0, (sockaddr *) &addrDest, sizeof(addrDest));


    if (rec_init_info == SOCKET_ERROR) {
        fprintf(stderr, "Socket error!");
        return 1;
    }

    char *buff_ptr = &(buffer_rx[0]);
    char copy_for_crc[BUFFERS_LEN];
    strcpy(copy_for_crc, buffer_rx);

    // Goto flag to read init packet again in case of CRC comparison failure
    read_init_packet_again:

    // Reads file name
    //-----------------------------------------------------------
    static char *fname;
    int strip_res = strip_data(&buff_ptr, (char *)NAME, &fname);
    if (strip_res != 0) {
        fprintf(stderr, "Error: cannot read file name\n");
        return 1;
    }

    // Reads file size
    //-----------------------------------------------------------
    int integer_fsize;
    char *fsize;
    strip_res = strip_data(&buff_ptr, (char *)SIZE, &fsize);
    if (strip_res != 0) {
        fprintf(stderr, "Error: cannot read file size\n");
        return 1;
    }

    // Reads sha
    //-----------------------------------------------------------
    char *sha;
    strip_res = strip_data(&buff_ptr, (char *)SHA, &sha);
    if (strip_res != 0) {
        fprintf(stderr, "Error: cannot read SHA\n");
        return 1;
    }

    // Reads init CRC
    //-----------------------------------------------------------
    char *str_init_crc;
    int init_crc;
    strip_res = strip_data(&buff_ptr, (char *)CRC, &str_init_crc);
    if (strip_res == 0) {
        init_crc = convert_c_str_to_int(str_init_crc);
    } else {
        fprintf(stderr, "Error: cannot read CRC\n");
        return 1;
    }

    // Calculates init CRC
    //-----------------------------------------------------------
    int offset = strlen(copy_for_crc) - (strlen(str_init_crc) + sizeof(CRC));
    copy_for_crc[offset] = '\0';
    int my_crc = get_crc(copy_for_crc, strlen(copy_for_crc), 0xffff, 0);

    // Checks CRC
    //-----------------------------------------------------------
    FILE *output;
    if (my_crc == init_crc) {
        printf("Init CRCs are equal!\n");

        output = fopen("output.png", "wb");
        printf("File name: %s\n", fname);

        integer_fsize = convert_c_str_to_int(fsize);
        printf("File size: %d\n", integer_fsize);

        printf("SHA: %s\n", sha);

        sendto(socketS, ACK, strlen(ACK), 0, (sockaddr *) &addrDest, sizeof(addrDest));
    } else {
        sendto(socketS, NOT_ACK, strlen("test"), 0, (sockaddr *) &addrDest, sizeof(addrDest));
        printf( "Init CRCs are not equal! - packet not accepted\n");
        goto read_init_packet_again;
    }

    // Recieves START flag
    // -----------------------------------------------------------------
    read_start_flag_again:

    clear_buffer(buffer_rx, BUFFERS_LEN);
    printf("Waiting for start flag...\n");
    int rec_start = recvfrom(socketS, buffer_rx, sizeof(buffer_rx),
                             0, (struct sockaddr *) &from,
                             &from_len);

    if (rec_start == SOCKET_ERROR && strncmp(buffer_rx, START, sizeof(START) - 1) == 0) {
        fprintf(stderr, "Socket error.\n");
        return 1;

    } else if (strncmp(buffer_rx, START, sizeof(START) - 1) == 0){
        printf("Start flag received - ready for data\n");
        sendto(socketS, ACK, strlen(ACK), 0, (sockaddr *) &addrDest, sizeof(addrDest));

    } else {
        printf( "Didn't receive START flag! - packet not accepted\n");
        sendto(socketS, NOT_ACK, strlen("test"), 0, (sockaddr *) &addrDest, sizeof(addrDest));
        goto read_start_flag_again;
    }

    // Reads data
    // -----------------------------------------------------------------
    while (true) {
        clear_buffer(buffer_rx, BUFFERS_LEN);
        int rec = recvfrom(socketS, buffer_rx, sizeof(buffer_rx),
                           0, (struct sockaddr *) &from,
                           &from_len);

        if (rec == SOCKET_ERROR) {
            fprintf(stderr, "Socket error.\n");
            return 1;
        }

        if (strncmp(buffer_rx, STOP, sizeof(STOP) - 1) == 0) {
            printf("Stop flag received!\n");
            break;
        }

        int real_data_size = BUFFERS_LEN - sizeof(DATA) - CRC_LEN; // odecitani a pricitani jednicky = 0
        int packet_size = integer_fsize > real_data_size ? (BUFFERS_LEN - CRC_LEN) : (integer_fsize + sizeof(DATA));


        // TODO Proč poslední string s CRC a N začíná na }?
        char *packet_num_and_crc = &(buffer_rx[packet_size]);

        // tohle funguje jako záplata, ale asi bychom to měli vyřešit jinak
        // smazat jen tohle
        bool flag = false;
        if (packet_size == (integer_fsize + sizeof(DATA))) {
            packet_num_and_crc = &(buffer_rx[packet_size + 1]);
            packet_size += 1;
            flag = true;
        }
        // end

        char *str_packet_num;
        int packet_num;
        strip_res = strip_data(&packet_num_and_crc, (char *)NUMBER, &str_packet_num);
        if (strip_res != 0) {
            fprintf(stderr, "Error: cannot read packet number\n");
            return 1;
        }

        char *str_data_crc;
        int data_crc;
        strip_res = strip_data(&packet_num_and_crc, (char *)CRC, &str_data_crc);
        if (strip_res == 0) {
            data_crc = convert_c_str_to_int(str_data_crc);
        } else {
            fprintf(stderr, "Error: cannot read data CRC\n");
            return 1;
        }

        //printf("Packet number: %d CRC: %d\n", packet_num, data_crc);
        char* arr_for_crc = new char[packet_size];
        for (int i = 0; i < packet_size; ++i) {
            arr_for_crc[i] = buffer_rx[i];
        }
        int my_crc = get_crc(arr_for_crc, packet_size, 0xffff, 0);

        //FILE *f = fopen("my_crc_buf.txt", "w");
        //fwrite(arr_for_crc, packet_size, 1, f);
        //fclose(f);

        if (data_crc == my_crc) {
            // TODO další záplata pro poslední packet (pak vymazat)
            if (flag) {
                packet_size -= 1;
            }
            write_file(buffer_rx, packet_size, output);
            packet_num = convert_c_str_to_int(str_packet_num);

            sendto(socketS, ACK, strlen("test"), 0, (sockaddr *) &addrDest, sizeof(addrDest));
        } else {
            sendto(socketS, NOT_ACK, strlen("test"), 0, (sockaddr *) &addrDest, sizeof(addrDest));
            printf( "CRCs are not equal! - packet not accepted\n");
            continue;
            // send NOT_ACK
        }

        integer_fsize -= real_data_size;
    }
    fclose(output);
    closesocket(socketS);

    return 0;
}

int strip_data(char **buff_ptr, char *tag, char **data) {
    int ret = 0;
    if (strncmp(*buff_ptr, tag, sizeof(tag) - 1) == 0) {
        *data = strtok(*buff_ptr, "{");
        *data = strtok(nullptr, "}");
        *buff_ptr = strtok(nullptr, "");
    } else {
        fprintf(stderr, "Error: invalid tag!\n");
        ret = 1;
    }
    return ret;
}
