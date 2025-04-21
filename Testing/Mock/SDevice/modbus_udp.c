#include "modbus_udp.h"
#include "Bindings/io.h"

#include <memory.h>

SDEVICE_HANDLE(ModbusUdp) * ModbusUdpSDeviceCreateInstance(void)
{
   SDEVICE_INIT_DATA(ModbusUdp) init =
   {
      .Base =
      {
         .ReadOperation  = MockReadOperation,
         .WriteOperation = MockWriteOperation
      },
   };

   SDEVICE_HANDLE(ModbusUdp) *handle = SDEVICE_CREATE_HANDLE(ModbusUdp)(&init, NULL);

   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   return handle;
}
