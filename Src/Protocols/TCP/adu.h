#pragma once

#include "../../PDU/pdu.h"

#define __MODBUS_TCP_MAX_ADU_SIZE (__MODBUS_MAX_PDU_SIZE + offsetof(ModbusTcpAduData, Pdu))

typedef struct  __attribute__((scalar_storage_order("big-endian"), packed))
{
   uint16_t TransactionId;
   uint16_t ProtocolId;
   uint16_t PacketSize;
   uint8_t SlaveAddress;
   ModbusCommonPdu Pdu;
} ModbusTcpAduData;

size_t ModbusTcpProcessAdu(__SDEVICE_HANDLE(Modbus) *, size_t);
