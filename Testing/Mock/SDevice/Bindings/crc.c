#include "crc.h"

uint16_t MockComputeCrc16(SDEVICE_HANDLE(ModbusRtu) *this, const void *data, size_t size)
{
   return 0xEEEE;
}
