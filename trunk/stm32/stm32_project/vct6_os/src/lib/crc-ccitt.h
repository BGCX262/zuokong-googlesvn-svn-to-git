#ifndef __CRC_CCITT_H
#define __CRC_CCITT_H

#include "stdint.h"

extern uint16 const crc_ccitt_table[256];

extern uint16 crc_ccitt(uint16 crc, const uint8 *buffer, uint32 len);

static __inline uint16 crc_ccitt_byte(uint16 crc, const uint8 c)
{
    return (crc >> 8) ^ crc_ccitt_table[(crc ^ c) & 0xff];
}

#endif /* __CRC_CCITT_H */
