#include "test_device.h"
#include "Mock/Functions/mock_functions.h"

#include <memory.h>

#define __MODBUS_MAX_BUFFER_SIZE (__MODBUS_TCP_MAX_MESSAGE_SIZE > __MODBUS_RTU_MAX_MESSAGE_SIZE ?                      \
                                  __MODBUS_TCP_MAX_MESSAGE_SIZE : __MODBUS_RTU_MAX_MESSAGE_SIZE)

static __SDEVICE_CONSTANT_DATA(ModbusRtu) RtuConstantData =
{
   .Common.ReadRegisters = ReadRegistersMock,
   .Common.WriteRegisters = WriteRegistersMock,
};

static __SDEVICE_CONSTANT_DATA(ModbusTcp) TcpConstantData =
{
   .Common.ReadRegisters = ReadRegistersMock,
   .Common.WriteRegisters = WriteRegistersMock,
};

void CreateModbusRtuSDevice(__SDEVICE_HANDLE(ModbusRtu) *handle)
{
   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   handle->Constant = &RtuConstantData;
   __SDEVICE_INITIALIZE_HANDLE(ModbusRtu)(handle);
}

void CreateModbusTcpSDevice(__SDEVICE_HANDLE(ModbusTcp) *handle)
{
   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   handle->Constant = &TcpConstantData;
   __SDEVICE_INITIALIZE_HANDLE(ModbusTcp)(handle);
}
