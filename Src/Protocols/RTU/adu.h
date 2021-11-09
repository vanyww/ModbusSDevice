#pragma once

#include "../../PDU/pdu.h"
#include "CRC/modbus_crc.h"

#define __MODBUS_RTU_EMPTY_ADU __MODBUS_RTU_ADU_STRUCT_DECLARATION(sizeof(ModbusCommonPdu),)
#define __MODBUS_RTU_EMPTY_ADU_SIZE (sizeof(__MODBUS_RTU_EMPTY_ADU) - sizeof(ModbusCommonPdu))
#define __MODBUS_RTU_ADU_STRUCT_DECLARATION(pdu_size, name)                                                            \
struct __attribute__((packed)) name                                                                                    \
{                                                                                                                      \
   uint8_t SlaveAddress;                                                                                               \
   union                                                                                                               \
   {                                                                                                                   \
      ModbusCommonPdu Pdu;                                                                                             \
      uint8_t PduBytes[(pdu_size)];                                                                                    \
   };                                                                                                                  \
   ModbusRtuCrcType Crc16;                                                                                             \
}

size_t ModbusRtuProcessAdu(__SDEVICE_HANDLE(Modbus) *, size_t);
