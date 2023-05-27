#include "private_static_rtu.h"

SDEVICE_RUNTIME_DATA_FORWARD_DECLARATION(ModbusRtuStaticInternal);
SDEVICE_INIT_DATA_FORWARD_DECLARATION(ModbusRtuStaticInternal);

SDEVICE_RUNTIME_DATA_DECLARATION(ModbusRtuStaticInternal) { };
SDEVICE_INIT_DATA_DECLARATION(ModbusRtuStaticInternal) { };

SDEVICE_HANDLE_DECLARATION(ModbusRtuStaticInternal);
SDEVICE_STRING_NAME_DEFINITION(ModbusRtuStaticInternal);

SDEVICE_HANDLE(ModbusRtuStaticInternal) *const ModbusRtuStaticInternalSDeviceHandle =
&(SDEVICE_HANDLE(ModbusRtuStaticInternal))
{
   .Header =
   {
      .Context           = MODBUS_RTU_SDEVICE_STATIC_INTERNAL_HANDLE_CONTEXT,
      .OwnerHandle       = NULL,
      .SDeviceStringName = SDEVICE_STRING_NAME(ModbusRtuStaticInternal),
      .LatestStatus      = MODBUS_RTU_SDEVICE_STATUS_OK,
      .Identifier        = MODBUS_RTU_SDEVICE_STATIC_INTERNAL_HANDLE_IDENTIFIER
   }
};
