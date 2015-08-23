#ifndef __CRC7_H
#define __CRC7_H

#include "stdint.h"

extern const uint8 crc7_syndrome_table[256];

static __inline uint8 crc7_byte(uint8 crc, uint8 data)
{
    return crc7_syndrome_table[(crc << 1) ^ data];
}

extern uint8 crc7(uint8 crc, const uint8 *buffer, uint32 len);

#endif
