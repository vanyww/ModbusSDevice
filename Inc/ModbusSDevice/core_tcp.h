#pragma once

#include "common.h"

#define __MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE 7U
#define __MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE 260U /* 253 (max PDU) + 7 (common ADU members) */

typedef struct
{
   ModbusSDeviceModbusType ModbusType;
   uint8_t SlaveAddress;
} ModbusTcpSDeviceRequestData;

/* Satty's interface start */

__SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusTcp);

typedef struct
{
   __SDEVICE_CONSTANT_DATA(Modbus) ModbusCommon;
} __SDEVICE_CONSTANT_DATA(ModbusTcp);

typedef union { } __SDEVICE_SETTINGS_DATA(ModbusTcp);

typedef struct
{
   uint8_t MbapHeaderBuffer[__MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE];
} __SDEVICE_DYNAMIC_DATA(ModbusTcp);

__SDEVICE_HANDLE_DEFINITION(ModbusTcp);

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(ModbusTcp,);

/* Satty's interface end */

bool ModbusTcpSDeviceTryProcessMbapHeader(__SDEVICE_HANDLE(ModbusTcp) *, ModbusSDeviceRequest *, size_t *);
bool ModbusTcpSDeviceTryProcessRequest(__SDEVICE_HANDLE(ModbusTcp) *, ModbusSDeviceRequest *, ModbusSDeviceResponse *);
