#include "test_device.h"
#include "Mock/Functions/mock_functions.h"

#include <memory.h>

#define __MODBUS_MAX_BUFFER_SIZE (__MODBUS_TCP_MAX_MESSAGE_SIZE > __MODBUS_RTU_MAX_MESSAGE_SIZE ?                      \
                                  __MODBUS_TCP_MAX_MESSAGE_SIZE : __MODBUS_RTU_MAX_MESSAGE_SIZE)

__SDEVICE_HANDLE(ModbusRtu) CreateModbusRtuSDevice(void)
{
   __SDEVICE_HANDLE(ModbusRtu) handle =
   {
      .Init = (__SDEVICE_INIT_DATA(ModbusRtu))
      {
         .Common.ReadRegisters = ReadRegistersMock,
         .Common.WriteRegisters = WriteRegistersMock,
      }
   };

   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   __SDEVICE_INITIALIZE_HANDLE(ModbusRtu)(&handle);

   return handle;
}

__SDEVICE_HANDLE(ModbusTcp) CreateModbusTcpSDevice(void)
{
   __SDEVICE_HANDLE(ModbusTcp) handle =
   {
      .Init = (__SDEVICE_INIT_DATA(ModbusTcp))
      {
         .Common.ReadRegisters = ReadRegistersMock,
         .Common.WriteRegisters = WriteRegistersMock,
      }
   };

   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   __SDEVICE_INITIALIZE_HANDLE(ModbusTcp)(&handle);

   return handle;
}
