#include "test_device.h"
#include "ModbusSDevice/RTU/rtu_defs.h"
#include "ModbusSDevice/TCP/tcp_defs.h"
#include "Mock/Functions/mock_functions.h"

#include <memory.h>

#define __MODBUS_MAX_BUFFER_SIZE (__MODBUS_SDEVICE_TCP_MAX_MESSAGE_SIZE > __MODBUS_SDEVICE_RTU_MAX_MESSAGE_SIZE ?      \
                                  __MODBUS_SDEVICE_TCP_MAX_MESSAGE_SIZE : __MODBUS_SDEVICE_RTU_MAX_MESSAGE_SIZE)

static uint8_t TransmitBuffer[__MODBUS_MAX_BUFFER_SIZE];
static uint8_t ReceiveBuffer[__MODBUS_MAX_BUFFER_SIZE];

__SDEVICE_HANDLE(Modbus) CreateModbusSDevice(ModbusSDeviceType type)
{
   memset(TransmitBuffer, 0, sizeof(TransmitBuffer));
   memset(ReceiveBuffer, 0, sizeof(ReceiveBuffer));
   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));

   __SDEVICE_HANDLE(Modbus) handle =
   {
      {
         .ReadRegistersFunction = ReadRegistersMock,
         .WriteRegistersFunction = WriteRegistersMock,
         .TransmitBuffer = TransmitBuffer,
         .ReceiveBuffer = ReceiveBuffer,
         .Type = type
      }
   };

   __SDEVICE_INITIALIZE_HANDLE(Modbus)(&handle);

   return handle;
}
