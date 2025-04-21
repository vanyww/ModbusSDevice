#include "modbus_rtu.h"
#include "Bindings/io.h"

#include <memory.h>

SDEVICE_HANDLE(ModbusRtu) * ModbusRtuSDeviceCreateInstance(void)
{
   SDEVICE_INIT_DATA(ModbusRtu) init =
   {
      .Base =
      {
         .ReadOperation  = MockReadOperation,
         .WriteOperation = MockWriteOperation
      },
   };

   SDEVICE_HANDLE(ModbusRtu) *handle = SDEVICE_CREATE_HANDLE(ModbusRtu)(&init, NULL);

   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   return handle;
}
