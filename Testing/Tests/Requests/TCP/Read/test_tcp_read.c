#include "test_tcp_read.h"
#include "../../../../Mock/Errors/errors.h"
#include "../../../../Mock/SDevice/test_device.h"
#include "../../../../Mock/SDevice/Bindings/bindings.h"

#include <memory.h>

bool TestTcpReadOneRequest(void)
{
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusTcp))))
         SDEVICE_HANDLE(ModbusTcp) *handle = CreateModbusTcpSDevice();

   MockReadRegisters[0] = 0x2211;

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x01, 0x03, 0x02, 0x11, 0x22 };
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

   if(WAS_ANY_ERROR_THROWN())
      return false;

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, replyBuffer, replySize) != 0)
      return false;

   return true;
}

bool TestTcpReadMultipleRequest(void)
{
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusTcp))))
         SDEVICE_HANDLE(ModbusTcp) *handle = CreateModbusTcpSDevice();

   MockReadRegisters[0] = 0x2211;
   MockReadRegisters[1] = 0x4433;

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x02 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x01, 0x03, 0x04, 0x11, 0x22, 0x33, 0x44 };
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

   if(WAS_ANY_ERROR_THROWN())
      return false;

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, replyBuffer, replySize) != 0)
      return false;

   return true;
}

bool TestTcpReadTooManyRequest(void)
{
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusTcp))))
         SDEVICE_HANDLE(ModbusTcp) *handle = CreateModbusTcpSDevice();

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0xFF };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x01, 0x83, 0x03 };
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

   if(!WAS_ANY_ERROR_THROWN())
      return false;

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, replyBuffer, replySize) != 0)
      return false;

   return true;
}
