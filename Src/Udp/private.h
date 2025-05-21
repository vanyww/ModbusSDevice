#pragma once

#include "ModbusSDevice/Udp/public.h"
#include "../internal.h"

SDEVICE_RUNTIME_DATA_FORWARD_DECLARATION(ModbusUdp);

SDEVICE_INTERNAL_ALIASES_DECLARATION(ModbusUdp);

SDEVICE_RUNTIME_DATA_DECLARATION(ModbusUdp)
{
   SDEVICE_PROPERTY_TYPE(ModbusUdp, BtuAddress) BtuAddress;
};

SDEVICE_HANDLE_DECLARATION(ModbusUdp);
