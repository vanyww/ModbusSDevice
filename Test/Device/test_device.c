#include "test_device.h"
#include "ModbusSDevice/rtu_defs.h"
#include "ModbusSDevice/tcp_defs.h"
#include "Mock/Functions/mock_functions.h"

#include <memory.h>

#define __MODBUS_MAX_BUFFER_SIZE (__MODBUS_SDEVICE_TCP_MAX_MESSAGE_SIZE > __MODBUS_SDEVICE_RTU_MAX_MESSAGE_SIZE ?      \
                                  __MODBUS_SDEVICE_TCP_MAX_MESSAGE_SIZE : __MODBUS_SDEVICE_RTU_MAX_MESSAGE_SIZE)

static __SDEVICE_CONSTANT_DATA(Modbus) ConstantData =
{
   .ReadRegistersFunction = ReadRegistersMock,
   .WriteRegistersFunction = WriteRegistersMock,
};

void CreateModbusSDevice(ModbusSDeviceType type, __SDEVICE_HANDLE(Modbus) *handle)
{
   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   ConstantData.Type = type;

   handle->Constant = &ConstantData;
   __SDEVICE_INITIALIZE_HANDLE(Modbus)(handle);
}
