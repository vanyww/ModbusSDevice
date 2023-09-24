#include "test_device.h"
#include "Bindings/bindings.h"

#include <memory.h>

#define __MODBUS_MAX_BUFFER_SIZE (__MODBUS_TCP_MAX_MESSAGE_SIZE > __MODBUS_RTU_MAX_MESSAGE_SIZE ?                      \
                                  __MODBUS_TCP_MAX_MESSAGE_SIZE : __MODBUS_RTU_MAX_MESSAGE_SIZE)

SDEVICE_HANDLE(ModbusRtu) * CreateModbusRtuSDevice(void)
{

   SDEVICE_INIT_DATA(ModbusRtu) init = { { ReadRegistersMock, WriteRegistersMock } };
   SDEVICE_HANDLE(ModbusRtu) *handle = SDEVICE_CREATE_HANDLE(ModbusRtu)(&init, NULL, 0, NULL);

   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   return handle;
}

SDEVICE_HANDLE(ModbusTcp) * CreateModbusTcpSDevice(void)
{
   SDEVICE_INIT_DATA(ModbusTcp) init = { { ReadRegistersMock, WriteRegistersMock } };
   SDEVICE_HANDLE(ModbusTcp) *handle = SDEVICE_CREATE_HANDLE(ModbusTcp)(&init, NULL, 0, NULL);

   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   return handle;
}

const uint8_t ModbusUdpBtuAddress[] = { 1, 2, 3, 4, 5, 6 };

SDEVICE_HANDLE(ModbusUdp) * CreateModbusUdpSDevice(void)
{
   SDEVICE_INIT_DATA(ModbusUdp) init = { { ReadRegistersMock, WriteRegistersMock }, ModbusUdpBtuAddress};
   SDEVICE_HANDLE(ModbusUdp) *handle = SDEVICE_CREATE_HANDLE(ModbusUdp)(&init, NULL, 0, NULL);

   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   return handle;
}

