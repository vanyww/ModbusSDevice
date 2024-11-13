#pragma once

#include "ModbusSDevice/Udp/public.h"
#include "../Base/private.h"

#define IS_VALID_THIS_HANDLE(handle) SDEVICE_IS_VALID_HANDLE(ModbusUdp, handle)

SDEVICE_RUNTIME_DATA_FORWARD_DECLARATION(ModbusUdp);

SDEVICE_RUNTIME_DATA_DECLARATION(ModbusUdp)
{
   BaseRuntimeData Base;
   uint8_t         BtuAddress[MODBUS_UDP_SDEVICE_BTU_ADDRESS_SIZE];
};

SDEVICE_HANDLE_DECLARATION(ModbusUdp);
SDEVICE_INTERNAL_ALIASES_DECLARATION(ModbusUdp);

typedef ModbusUdpSDeviceInput ThisInput;
typedef ModbusUdpSDeviceOutput ThisOutput;

typedef ModbusUdpSDeviceCallParameters ThisCallParameters;
