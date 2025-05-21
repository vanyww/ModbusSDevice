#pragma once

#include "ModbusSDevice/Tcp/public.h"
#include "../internal.h"

SDEVICE_RUNTIME_DATA_FORWARD_DECLARATION(ModbusTcp);

SDEVICE_INTERNAL_ALIASES_DECLARATION(ModbusTcp);

SDEVICE_RUNTIME_DATA_DECLARATION(ModbusTcp)
{
   uint16_t MbapHeaderTransactionIdx;
   uint16_t MbapHeaderPacketSize;
   uint8_t  MbapHeaderSlaveAddress;
};

SDEVICE_HANDLE_DECLARATION(ModbusTcp);
