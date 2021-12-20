#include "test_tcp_write.h"
#include "../../../../Device/test_device.h"
#include "../../../../Device/Mock/Assertation/mock_assert.h"
#include "../../../../Device/Mock/Functions/mock_functions.h"
#include "../../../../Device/Mock/RuntimeError/mock_handle_runtime_error.h"

#include <memory.h>

bool TestTcpWriteOneRequest(void)
{
   __SDEVICE_HANDLE(ModbusTcp) handle = { 0 };
   CreateModbusTcpSDevice(&handle);

   const uint8_t request[] =
            { 0x00, 0x01, 0x00, 0x00, 0x00, 0x09, 0x01, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x11, 0x22 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x10, 0x00, 0x00, 0x00, 0x01 };
   ModbusSDeviceRequest mbapData = { .Bytes = request, .BytesCount = __MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE };
   ModbusSDeviceRequest requestData =
   {
      .Bytes = request + __MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE,
      .BytesCount = sizeof(request) - __MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE
   };
   ModbusSDeviceResponse responseData = { .Bytes = (uint8_t[__MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE]){ } };

   size_t packetSize;

   if(ModbusTcpSDeviceTryProcessMbapHeader(&handle, &mbapData, &packetSize) != true)
      return false;

   if(ModbusTcpSDeviceTryProcessRequest(&handle, &requestData, &responseData) != true)
      return false;

   if(responseData.BytesCount != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Bytes, responseData.BytesCount) != 0)
      return false;

   if(MockWriteRegisters[0].AsValue != 0x1122)
      return false;

   if(WasAssertFailed() != false)
      return false;

   if(WasRuntimeErrorRaised() == true)
      return false;

   return true;
}

bool TestTcpWriteMultipleRequest(void)
{
   __SDEVICE_HANDLE(ModbusTcp) handle = { 0 };
   CreateModbusTcpSDevice(&handle);

   const uint8_t request[] =
            { 0x00, 0x01, 0x00, 0x00, 0x00, 0x0B, 0x01, 0x10, 0x00, 0x00, 0x00, 0x02, 0x04, 0x11, 0x22, 0x33, 0x44 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x10, 0x00, 0x00, 0x00, 0x02 };
   ModbusSDeviceRequest mbapData = { .Bytes = request, .BytesCount = __MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE };
   ModbusSDeviceRequest requestData =
   {
      .Bytes = request + __MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE,
      .BytesCount = sizeof(request) - __MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE
   };
   ModbusSDeviceResponse responseData = { .Bytes = (uint8_t[__MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE]){ } };

   size_t packetSize;

   if(ModbusTcpSDeviceTryProcessMbapHeader(&handle, &mbapData, &packetSize) != true)
      return false;

   if(ModbusTcpSDeviceTryProcessRequest(&handle, &requestData, &responseData) != true)
      return false;

   if(responseData.BytesCount != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Bytes, responseData.BytesCount) != 0)
      return false;

   if(MockWriteRegisters[0].AsValue != 0x1122)
      return false;

   if(MockWriteRegisters[1].AsValue != 0x3344)
      return false;

   if(WasAssertFailed() != false)
      return false;

   if(WasRuntimeErrorRaised() == true)
      return false;

   return true;
}
