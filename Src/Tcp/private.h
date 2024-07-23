#pragma once

#include "ModbusSDevice/Tcp/public.h"
#include "../Base/private.h"

#define IS_VALID_THIS_HANDLE(handle) SDEVICE_IS_VALID_HANDLE(ModbusTcp, handle)

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

typedef ModbusTcpSDeviceOperationContext ThisOperationContext;
