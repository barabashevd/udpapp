//
// Created by barab on 05.03.2021.
//

// ZDROJ: http://automationwiki.com/index.php?title=CRC-16-CCITT

#ifndef UDPAPP_CRC_H
#define UDPAPP_CRC_H

unsigned short get_crc(char *data, size_t length, unsigned short seed, unsigned short final);

#endif //UDPAPP_CRC_H
