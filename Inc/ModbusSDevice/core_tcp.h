#pragma once

#include "core_common.h"

#define __MODBUS_TCP_MBAP_HEADER_SIZE 7U
#define __MODBUS_TCP_MAX_MESSAGE_SIZE 260U /* 253 (max PDU) + 7 (common ADU members) */

typedef struct
{
   ModbusRequestContext Common;
   uint8_t SlaveAddress;
} ModbusTcpRequestContext;

/* Satty's interface start */

__SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusTcp);
__SDEVICE_RUNTIME_DATA_FORWARD_DECLARATION(ModbusTcp);

typedef struct
{
   __SDEVICE_INIT_DATA(Modbus) Common;
} __SDEVICE_INIT_ARGUMENTS(ModbusTcp);

typedef __SDEVICE_INIT_ARGUMENTS(ModbusTcp) __SDEVICE_INIT_DATA(ModbusTcp);

__SDEVICE_HANDLE_DEFINITION(ModbusTcp);

__SDEVICE_CREATE_HANDLE_DECLARATION(ModbusTcp,,,);
__SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusTcp,);

/* Satty's interface end */

bool ModbusTcpTryProcessMbapHeader(__SDEVICE_HANDLE(ModbusTcp) *, ModbusRequest *, size_t *);
bool ModbusTcpTryProcessRequest(__SDEVICE_HANDLE(ModbusTcp) *, ModbusRequest *, ModbusResponse *);
