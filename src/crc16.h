/* $Id: crc16.h,v 1.2 2004/07/11 09:32:05 ozzmosis Exp $ */

unsigned short CRC16DoByte(unsigned char byte, unsigned short CRCnow);
unsigned short CRC16String(const char *str, unsigned short crc);
