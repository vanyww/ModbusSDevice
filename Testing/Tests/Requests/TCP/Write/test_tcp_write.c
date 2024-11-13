#include "test_tcp_write.h"
#include "../../../../Mock/Errors/errors.h"
#include "../../../../Mock/SDevice/test_device.h"
#include "../../../../Mock/SDevice/Bindings/bindings.h"

#include <memory.h>

bool TestTcpWriteOneRequest(void)
{
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusTcp))))
         SDEVICE_HANDLE(ModbusTcp) *handle = CreateModbusTcpSDevice();

   const uint8_t request[] =
            { 0x00, 0x01, 0x00, 0x00, 0x00, 0x09, 0x01, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x11, 0x22 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x10, 0x00, 0x00, 0x00, 0x01 };
   uint8_t replyBuffer[MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE];
   size_t packetSize;
   size_t replySize;

   ModbusTcpSDeviceInput input =
   {
      request + MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE,
      sizeof(request) - MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE
   };
   ModbusTcpSDeviceOutput output = { replyBuffer, &replySize };

   if(ModbusTcpSDeviceTryProcessMbapHeader(handle, request, &packetSize) != true)
      return false;

   if(ModbusTcpSDeviceTryProcessRequest(handle, input, output) != true)
      return false;

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, replyBuffer, replySize) != 0)
      return false;

   if(MockWriteRegisters[0] != 0x2211)
      return false;

   if(WAS_ANY_ERROR_THROWN())
      return false;

   return true;
}

bool TestTcpWriteMultipleRequest(void)
{
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusTcp))))
         SDEVICE_HANDLE(ModbusTcp) *handle = CreateModbusTcpSDevice();

   const uint8_t request[] =
            { 0x00, 0x01, 0x00, 0x00, 0x00, 0x0B, 0x01, 0x10, 0x00, 0x00, 0x00, 0x02, 0x04, 0x11, 0x22, 0x33, 0x44 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x10, 0x00, 0x00, 0x00, 0x02 };
   uint8_t replyBuffer[MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE];
   size_t packetSize;
   size_t replySize;

   ModbusTcpSDeviceInput input =
   {
      request + MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE,
      sizeof(request) - MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE
   };
   ModbusTcpSDeviceOutput output = { replyBuffer, &replySize };

   if(ModbusTcpSDeviceTryProcessMbapHeader(handle, request, &packetSize) != true)
      return false;

   if(ModbusTcpSDeviceTryProcessRequest(handle, input, output) != true)
      return false;

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, replyBuffer, replySize) != 0)
      return false;

   if(MockWriteRegisters[0] != 0x2211)
      return false;

   if(MockWriteRegisters[1] != 0x4433)
      return false;

   if(WAS_ANY_ERROR_THROWN())
      return false;

   return true;
}
