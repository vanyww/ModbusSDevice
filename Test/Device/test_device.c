#include "test_device.h"
#include "Mock/Functions/mock_functions.h"

#include <memory.h>

#define __MODBUS_MAX_BUFFER_SIZE (__MODBUS_TCP_MAX_MESSAGE_SIZE > __MODBUS_RTU_MAX_MESSAGE_SIZE ?                      \
                                  __MODBUS_TCP_MAX_MESSAGE_SIZE : __MODBUS_RTU_MAX_MESSAGE_SIZE)

__SDEVICE_HANDLE(ModbusRtu) CreateModbusRtuSDevice(void)
{
   __SDEVICE_INIT_ARGUMENTS(ModbusRtu) arguments =
   {
      .Common.ReadRegisters = ReadRegistersMock,
      .Common.WriteRegisters = WriteRegistersMock,
   };

   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   return __SDEVICE_CREATE_HANDLE(ModbusRtu)(&arguments, 0, NULL);
}

__SDEVICE_HANDLE(ModbusTcp) CreateModbusTcpSDevice(void)
{
   __SDEVICE_INIT_ARGUMENTS(ModbusTcp) arguments =
   {
      .Common.ReadRegisters = ReadRegistersMock,
      .Common.WriteRegisters = WriteRegistersMock,
   };

   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   return __SDEVICE_CREATE_HANDLE(ModbusTcp)(&arguments, 0, NULL);
}
