#include "modbus_tcp.h"
#include "Bindings/io.h"

#include <memory.h>

SDEVICE_HANDLE(ModbusTcp) * ModbusTcpSDeviceCreateInstance(void)
{
   SDEVICE_INIT_DATA(ModbusTcp) init =
   {
      .Base =
      {
         .ReadOperation  = MockReadOperation,
         .WriteOperation = MockWriteOperation
      },
   };

   SDEVICE_HANDLE(ModbusTcp) *handle = SDEVICE_CREATE_HANDLE(ModbusTcp)(&init, NULL);

   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   return handle;
}
