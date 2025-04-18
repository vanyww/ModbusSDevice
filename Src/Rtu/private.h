#pragma once

#include "ModbusSDevice/Rtu/public.h"
#include "../Base/private.h"

SDEVICE_RUNTIME_DATA_FORWARD_DECLARATION(ModbusRtu);

SDEVICE_RUNTIME_DATA_DECLARATION(ModbusRtu)
{
   BaseRuntimeData                                Base;
   SDEVICE_PROPERTY_TYPE(ModbusRtu, SlaveAddress) SlaveAddress;
};

SDEVICE_HANDLE_DECLARATION(ModbusRtu);
SDEVICE_INTERNAL_ALIASES_DECLARATION(ModbusRtu);

typedef ModbusRtuSDeviceInput ThisInput;
typedef ModbusRtuSDeviceOutput ThisOutput;

typedef ModbusRtuSDeviceCallParameters ThisCallParameters;
