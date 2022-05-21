#pragma once

#include "core_common.h"

#define __MODBUS_TCP_MBAP_HEADER_SIZE 7U
#define __MODBUS_TCP_MAX_MESSAGE_SIZE 260U /* 253 (max PDU) + 7 (common ADU members) */

typedef struct
{
   ModbusRequestData Common;
   uint8_t SlaveAddress;
} ModbusTcpRequestData;

/* Satty's interface start */

__SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusTcp);

typedef struct
{
   __SDEVICE_INIT_DATA(Modbus) Common;
} __SDEVICE_INIT_DATA(ModbusTcp);

typedef struct
{
   uint8_t MbapHeaderBuffer[__MODBUS_TCP_MBAP_HEADER_SIZE];
} __SDEVICE_RUNTIME_DATA(ModbusTcp);

__SDEVICE_HANDLE_DEFINITION(ModbusTcp);

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(ModbusTcp,);

/* Satty's interface end */

bool ModbusTcpTryProcessMbapHeader(__SDEVICE_HANDLE(ModbusTcp) *, ModbusRequest *, size_t *);
bool ModbusTcpTryProcessRequest(__SDEVICE_HANDLE(ModbusTcp) *, ModbusRequest *, ModbusResponse *);
