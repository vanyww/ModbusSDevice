#include "test_tcp_read.h"
#include "../../../../Device/test_device.h"
#include "../../../../Device/Mock/Assertation/mock_assert.h"
#include "../../../../Device/Mock/Functions/mock_functions.h"
#include "ModbusSDevice/RTU/rtu_defs.h"

#include <memory.h>

bool TestTcpReadOneRequest(void)
{
   __SDEVICE_HANDLE(Modbus) handle = CreateModbusSDevice(MODBUS_SDEVICE_TYPE_TCP);

   MockReadRegisters[0] = (ModbusSDeviceRegister){ .AsValue = 0x1122 };

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x01, 0x03, 0x02, 0x11, 0x22 };

   memcpy(handle.Constant.ReceiveBuffer, request, sizeof(request));
   size_t replySize = ModbusSDeviceProcessRequest(&handle, sizeof(request));

   if(WasAssertFailed() != false)
      return false;

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, handle.Constant.TransmitBuffer, replySize) != 0)
      return false;

   return true;
}

bool TestTcpReadMultipleRequest(void)
{
   __SDEVICE_HANDLE(Modbus) handle = CreateModbusSDevice(MODBUS_SDEVICE_TYPE_TCP);

   MockReadRegisters[0] = (ModbusSDeviceRegister){ .AsValue = 0x1122 };
   MockReadRegisters[1] = (ModbusSDeviceRegister){ .AsValue = 0x3344 };

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x02 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x01, 0x03, 0x04, 0x11, 0x22, 0x33, 0x44 };

   memcpy(handle.Constant.ReceiveBuffer, request, sizeof(request));
   size_t replySize = ModbusSDeviceProcessRequest(&handle, sizeof(request));

   if(WasAssertFailed() != false)
      return false;

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, handle.Constant.TransmitBuffer, replySize) != 0)
      return false;

   return true;
}

bool TestTcpReadTooManyRequest(void)
{
   __SDEVICE_HANDLE(Modbus) handle = CreateModbusSDevice(MODBUS_SDEVICE_TYPE_TCP);

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0xFF };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x01, 0x83, 0x03 };

   memcpy(handle.Constant.ReceiveBuffer, request, sizeof(request));
   size_t replySize = ModbusSDeviceProcessRequest(&handle, sizeof(request));

   if(WasAssertFailed() != false)
      return false;

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, handle.Constant.TransmitBuffer, replySize) != 0)
      return false;

   return true;
}
