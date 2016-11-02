/* $Id: crc16.h,v 1.1.1.1 2009/01/08 20:07:46 mbroek Exp $ */

unsigned short CRC16DoByte(unsigned char byte, unsigned short CRCnow);
unsigned short CRC16String(const char *str, unsigned short crc);
