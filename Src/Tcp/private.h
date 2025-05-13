#pragma once

#include "ModbusSDevice/Tcp/public.h"
#include "../private.h"

SDEVICE_RUNTIME_DATA_FORWARD_DECLARATION(ModbusTcp);

SDEVICE_RUNTIME_DATA_DECLARATION(ModbusTcp)
{
   BaseRuntimeData Base;

   struct
   {
      uint16_t TransactionIdx;
      uint16_t PacketSize;
      uint8_t  SlaveAddress;
   } MbapHeaderData;
};

SDEVICE_HANDLE_DECLARATION(ModbusTcp);
SDEVICE_INTERNAL_ALIASES_DECLARATION(ModbusTcp);

typedef ModbusTcpSDeviceInput ThisInput;
typedef ModbusTcpSDeviceOutput ThisOutput;

typedef ModbusTcpSDeviceCallParameters ThisCallParameters;
