#pragma once

#include "ModbusSDevice/Rtu/public.h"

#if MODBUS_RTU_SDEVICE_USE_INTERNAL_CRC
extern SDEVICE_HANDLE(TableCrc16) *$ModbusRtuSDeviceCrc16Handle;
#endif

static inline void InitializeCrc16(void)
{
#if MODBUS_RTU_SDEVICE_USE_INTERNAL_CRC
   if($ModbusRtuSDeviceCrc16Handle)
      return;

   SDEVICE_INIT_DATA(TableCrc16) crc16Init =
   {
      .ExternalLookupTable = NULL,
      .Polynomial          = MODBUS_RTU_SDEVICE_CRC16_POLYNOMIAL,
      .InitialValue        = MODBUS_RTU_SDEVICE_CRC16_INITIAL_VALUE,
      .OutputXorValue      = MODBUS_RTU_SDEVICE_CRC16_OUTPUT_XOR_VALUE,
      .IsReverse           = MODBUS_RTU_SDEVICE_CRC16_IS_REVERSE
   };

   $ModbusRtuSDeviceCrc16Handle =
         SDEVICE_CREATE_HANDLE(TableCrc16)(&crc16Init, NULL, MODBUS_RTU_SDEVICE_INTERNAL_CRC16_HANDLE_IDENTIFIER, NULL);
#endif
}

static inline uint16_t ComputeCrc16(SDEVICE_HANDLE(ModbusRtu) *handle, const void *data, size_t size)
{
#if MODBUS_RTU_SDEVICE_USE_INTERNAL_CRC
   return TableCrc16SDeviceCompute($ModbusRtuSDeviceCrc16Handle, data, size);
#else
   return handle->Init.ComputeCrc16(handle, data, size);
#endif
}
