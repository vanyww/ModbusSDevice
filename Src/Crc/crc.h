#pragma once

#include "../private_rtu.h"

#include "SDeviceCore/errors.h"

#ifndef MODBUS_RTU_SDEVICE_USE_EXTERN_CRC
#include "TableCrcSDevice/public_crc16.h"

#if (TABLE_CRC_SDEVICE_VERSION_MAJOR != 3) || (TABLE_CRC_SDEVICE_VERSION_MINOR < 1)
#error Table CRC SDevice version is incorrect.
#endif

#ifndef MODBUS_RTU_SDEVICE_TABLE_CRC16_INTERNAL_SDEVICE_IDENTIFIER
#define MODBUS_RTU_SDEVICE_TABLE_CRC16_INTERNAL_SDEVICE_IDENTIFIER 0
#endif

extern SDEVICE_HANDLE(TableCrc16) *ModbusRtuSDeviceInternalCrc16Handle;
#endif

static inline void InitializeCrc16(void)
{
#ifndef MODBUS_RTU_SDEVICE_USE_EXTERN_CRC
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

   SDeviceHandleIdentifier identifier = MODBUS_RTU_SDEVICE_TABLE_CRC16_INTERNAL_SDEVICE_IDENTIFIER;
   ModbusRtuSDeviceInternalCrc16Handle = SDEVICE_CREATE_HANDLE(TableCrc16)(&crc16Init, NULL, identifier, NULL);
   SDeviceAssert(ModbusRtuSDeviceInternalCrc16Handle != NULL);
#endif
}

static inline uint16_t ComputeCrc16(SDEVICE_HANDLE(ModbusRtu) *handle, const void *data, size_t size)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(data != NULL || size == 0);

#ifndef MODBUS_RTU_SDEVICE_USE_EXTERN_CRC
   SDeviceDebugAssert(ModbusRtuSDeviceInternalCrc16Handle != NULL);

   return TableCrc16SDeviceCompute(ModbusRtuSDeviceInternalCrc16Handle, data, size);
#else
   return handle->Init.ComputeCrc16(handle, data, size);
#endif
}
