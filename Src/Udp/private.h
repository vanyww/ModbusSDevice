#pragma once

#include "ModbusSDevice/Udp/public.h"
#include "../Base/private.h"

SDEVICE_RUNTIME_DATA_FORWARD_DECLARATION(ModbusUdp);

SDEVICE_RUNTIME_DATA_DECLARATION(ModbusUdp)
{
   SDEVICE_RUNTIME_DATA(Modbus) BaseRuntime;
};

SDEVICE_HANDLE_DECLARATION(ModbusUdp);
SDEVICE_INTERNAL_ALIASES_DECLARATION(ModbusUdp);
