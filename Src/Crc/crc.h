#pragma once

#include "../static_internal_rtu.h"

#include "SDeviceCore/errors.h"

#if defined MODBUS_RTU_SDEVICE_USE_INTERNAL_CRC
#include "TableCrcSDevice/public_crc16.h"

#if (TABLE_CRC_SDEVICE_VERSION_MAJOR != 4) || (TABLE_CRC_SDEVICE_VERSION_MINOR < 0)
#error Table CRC SDevice version is incorrect.
#endif

extern SDEVICE_HANDLE(TableCrc16) *ModbusRtuSDeviceInternalCrc16Handle;
#endif

static inline void InitializeCrc16(void)
{
#if defined MODBUS_RTU_SDEVICE_USE_INTERNAL_CRC
   if(ModbusRtuSDeviceInternalCrc16Handle != NULL)
      return;

   SDEVICE_INIT_DATA(TableCrc16) crc16Init =
   {
      .ExternalLookupTable = NULL,
      .Polynomial = 0x8005,
      .InitialValue = 0xFFFF,
      .OutputXorValue = 0x0000,
      .IsReverse = true
   };

   SDeviceHandleIdentifier identifier = MODBUS_RTU_SDEVICE_INTERNAL_CRC16_HANDLE_IDENTIFIER;
   ModbusRtuSDeviceInternalCrc16Handle =
         SDEVICE_CREATE_HANDLE(TableCrc16)(&crc16Init, ModbusRtuStaticInternalSDeviceHandle, identifier, NULL);
#endif
}

static inline uint16_t ComputeCrc16(SDEVICE_HANDLE(ModbusRtu) *handle, const void *data, size_t size)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(data != NULL || size == 0);

#if defined MODBUS_RTU_SDEVICE_USE_INTERNAL_CRC
   SDeviceDebugAssert(ModbusRtuSDeviceInternalCrc16Handle != NULL);

   return TableCrc16SDeviceCompute(ModbusRtuSDeviceInternalCrc16Handle, data, size);
#else
   return handle->Init.ComputeCrc16(handle, data, size);
#endif
}
