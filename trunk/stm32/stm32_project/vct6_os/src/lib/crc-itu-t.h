/*
 *	crc-itu-t.h - CRC ITU-T V.41 routine
 *
 * Implements the standard CRC ITU-T V.41:
 *   Width 16
 *   Poly  0x0x1021 (x^16 + x^12 + x^15 + 1)
 *   Init  0
 *
 * This source code is licensed under the GNU General Public License,
 * Version 2. See the file COPYING for more details.
 */

#ifndef CRC_ITU_T_H
#define CRC_ITU_T_H

#include "stdint.h"

extern uint16 const crc_itu_t_table[256];

extern uint16 crc_itu_t(uint16 crc, const uint8 *buffer, uint32 len);

static __inline uint16 crc_itu_t_byte(uint16 crc, const uint8 data)
{
    return (crc << 8) ^ crc_itu_t_table[((crc >> 8) ^ data) & 0xff];
}

#endif /* CRC_ITU_T_H */

