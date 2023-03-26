#include "test_device.h"
#include "Mock/Functions/mock_functions.h"

#include <memory.h>

#define __MODBUS_MAX_BUFFER_SIZE (__MODBUS_TCP_MAX_MESSAGE_SIZE > __MODBUS_RTU_MAX_MESSAGE_SIZE ?                      \
                                  __MODBUS_TCP_MAX_MESSAGE_SIZE : __MODBUS_RTU_MAX_MESSAGE_SIZE)

SDEVICE_HANDLE(ModbusRtu) * CreateModbusRtuSDevice(void)
{

   SDEVICE_INIT_DATA(ModbusRtu) init = { { ReadRegistersMock, WriteRegistersMock } };
   SDEVICE_HANDLE(ModbusRtu) * handle = SDEVICE_CREATE_HANDLE(ModbusRtu)(&init, NULL, 0, NULL);

   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   return handle;
}

SDEVICE_HANDLE(ModbusTcp) * CreateModbusTcpSDevice(void)
{
   SDEVICE_INIT_DATA(ModbusTcp) init = { { ReadRegistersMock, WriteRegistersMock } };
   SDEVICE_HANDLE(ModbusTcp) * handle = SDEVICE_CREATE_HANDLE(ModbusTcp)(&init, NULL, 0, NULL);

   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   return handle;
}
