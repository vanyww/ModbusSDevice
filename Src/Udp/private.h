#pragma once

#include "ModbusSDevice/Udp/public.h"
#include "../Base/private.h"

#define IS_VALID_THIS_HANDLE(handle) (                                                                                 \
   {                                                                                                                   \
      ThisHandle *_handle = (handle);                                                                                  \
      _handle != NULL       &&                                                                                         \
      _handle->Init != NULL &&                                                                                         \
      SDeviceIsHandleOfIdentity(_handle, &SDEVICE_IDENTITY_BLOCK(ModbusUdp));                                          \
   })

SDEVICE_RUNTIME_DATA_FORWARD_DECLARATION(ModbusUdp);

SDEVICE_RUNTIME_DATA_DECLARATION(ModbusUdp)
{
   SDEVICE_RUNTIME_DATA(Modbus) BaseRuntime;
};

SDEVICE_HANDLE_DECLARATION(ModbusUdp);
SDEVICE_INTERNAL_ALIASES_DECLARATION(ModbusUdp);
