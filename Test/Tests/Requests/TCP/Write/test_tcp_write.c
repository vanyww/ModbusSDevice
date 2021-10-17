#include "test_tcp_write.h"
#include "../../../../Device/test_device.h"
#include "../../../../Device/Mock/Assertation/mock_assert.h"
#include "../../../../Device/Mock/Functions/mock_functions.h"
#include "ModbusSDevice/RTU/rtu_defs.h"

#include <memory.h>

bool TestTcpWriteOneRequest(void)
{
   __SDEVICE_HANDLE(Modbus) handle = CreateModbusSDevice(MODBUS_SDEVICE_TYPE_TCP);

   const uint8_t request[] =
            { 0x00, 0x01, 0x00, 0x00, 0x00, 0x09, 0x01, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x11, 0x22 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x10, 0x00, 0x00, 0x00, 0x01 };

   memcpy(handle.Constant.ReceiveBuffer, request, sizeof(request));
   size_t replySize = ModbusSDeviceProcessRequest(&handle, sizeof(request));

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, handle.Constant.TransmitBuffer, replySize) != 0)
      return false;

   if(MockWriteRegisters[0].AsValue != 0x1122)
      return false;

   if(WasAssertFailed() != false)
      return false;

   return true;
}

bool TestTcpWriteMultipleRequest(void)
{
   __SDEVICE_HANDLE(Modbus) handle = CreateModbusSDevice(MODBUS_SDEVICE_TYPE_TCP);

   const uint8_t request[] =
            { 0x00, 0x01, 0x00, 0x00, 0x00, 0x0B, 0x01, 0x10, 0x00, 0x00, 0x00, 0x02, 0x04, 0x11, 0x22, 0x33, 0x44 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x10, 0x00, 0x00, 0x00, 0x02 };

   memcpy(handle.Constant.ReceiveBuffer, request, sizeof(request));
   size_t replySize = ModbusSDeviceProcessRequest(&handle, sizeof(request));

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, handle.Constant.TransmitBuffer, replySize) != 0)
      return false;

   if(MockWriteRegisters[0].AsValue != 0x1122)
      return false;

   if(MockWriteRegisters[1].AsValue != 0x3344)
      return false;

   if(WasAssertFailed() != false)
      return false;

   return true;
}
