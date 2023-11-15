#pragma once

#include "ModbusSDevice/Rtu/public.h"
#include "../Base/private.h"

#define IS_VALID_THIS_HANDLE(handle) (                                                                                 \
   {                                                                                                                   \
      ThisHandle *_handle = (handle);                                                                                  \
      _handle != NULL          &&                                                                                      \
      _handle->Init != NULL    &&                                                                                      \
      _handle->Runtime != NULL &&                                                                                      \
      SDeviceIsHandleOfIdentity(_handle, &SDEVICE_IDENTITY_BLOCK(ModbusRtu));                                          \
   })

SDEVICE_RUNTIME_DATA_FORWARD_DECLARATION(ModbusRtu);

SDEVICE_RUNTIME_DATA_DECLARATION(ModbusRtu)
{
   SDEVICE_RUNTIME_DATA(Modbus)                   BaseRuntime;
   SDEVICE_PROPERTY_TYPE(ModbusRtu, SlaveAddress) SlaveAddress;
};

SDEVICE_HANDLE_DECLARATION(ModbusRtu);
SDEVICE_INTERNAL_ALIASES_DECLARATION(ModbusRtu);
