#include "test_tcp_read.h"
#include "../../../../Device/test_device.h"
#include "../../../../Device/Mock/Assertation/mock_assert.h"
#include "../../../../Device/Mock/Functions/mock_functions.h"
#include "../../../../Device/Mock/RuntimeError/mock_handle_runtime_error.h"

#include <memory.h>

bool TestTcpReadOneRequest(void)
{
   __SDEVICE_HANDLE(ModbusTcp) handle =  CreateModbusTcpSDevice();

   MockReadRegisters[0] = (ModbusRegister){ .AsValue = 0x1122 };

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x01, 0x03, 0x02, 0x11, 0x22 };
   ModbusRequest mbapData = { .Bytes = request, .Size = __MODBUS_TCP_MBAP_HEADER_SIZE };
   ModbusRequest requestData =
   {
      .Bytes = request + __MODBUS_TCP_MBAP_HEADER_SIZE,
      .Size = sizeof(request) - __MODBUS_TCP_MBAP_HEADER_SIZE
   };
   ModbusResponse responseData = { .Bytes = (uint8_t[__MODBUS_TCP_MAX_MESSAGE_SIZE]){ } };

   size_t packetSize;

   if(ModbusTcpTryProcessMbapHeader(&handle, &mbapData, &packetSize) != true)
      return false;

   if(ModbusTcpTryProcessRequest(&handle, &requestData, &responseData) != true)
      return false;

   if(WasAssertFailed() != false)
      return false;

   if(WasRuntimeErrorRaised() == true)
      return false;

   if(responseData.Size != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Bytes, responseData.Size) != 0)
      return false;

   return true;
}

bool TestTcpReadMultipleRequest(void)
{
   __SDEVICE_HANDLE(ModbusTcp) handle =  CreateModbusTcpSDevice();

   MockReadRegisters[0] = (ModbusRegister){ .AsValue = 0x1122 };
   MockReadRegisters[1] = (ModbusRegister){ .AsValue = 0x3344 };

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x02 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x01, 0x03, 0x04, 0x11, 0x22, 0x33, 0x44 };
   ModbusRequest mbapData = { .Bytes = request, .Size = __MODBUS_TCP_MBAP_HEADER_SIZE };
   ModbusRequest requestData =
   {
      .Bytes = request + __MODBUS_TCP_MBAP_HEADER_SIZE,
      .Size = sizeof(request) - __MODBUS_TCP_MBAP_HEADER_SIZE
   };
   ModbusResponse responseData = { .Bytes = (uint8_t[__MODBUS_TCP_MAX_MESSAGE_SIZE]){ } };

   size_t packetSize;

   if(ModbusTcpTryProcessMbapHeader(&handle, &mbapData, &packetSize) != true)
      return false;

   if(ModbusTcpTryProcessRequest(&handle, &requestData, &responseData) != true)
      return false;

   if(WasAssertFailed() != false)
      return false;

   if(WasRuntimeErrorRaised() == true)
      return false;

   if(responseData.Size != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Bytes, responseData.Size) != 0)
      return false;

   return true;
}

bool TestTcpReadTooManyRequest(void)
{
   __SDEVICE_HANDLE(ModbusTcp) handle =  CreateModbusTcpSDevice();

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0xFF };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x01, 0x83, 0x03 };
   ModbusRequest mbapData = { .Bytes = request, .Size = __MODBUS_TCP_MBAP_HEADER_SIZE };
   ModbusRequest requestData =
   {
      .Bytes = request + __MODBUS_TCP_MBAP_HEADER_SIZE,
      .Size = sizeof(request) - __MODBUS_TCP_MBAP_HEADER_SIZE
   };
   ModbusResponse responseData = { .Bytes = (uint8_t[__MODBUS_TCP_MAX_MESSAGE_SIZE]){ } };

   size_t packetSize;

   if(ModbusTcpTryProcessMbapHeader(&handle, &mbapData, &packetSize) != true)
      return false;

   if(ModbusTcpTryProcessRequest(&handle, &requestData, &responseData) != true)
      return false;

   if(WasAssertFailed() != false)
      return false;

   if(WasRuntimeErrorRaised() != true)
      return false;

   if(responseData.Size != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Bytes, responseData.Size) != 0)
      return false;

   return true;
}
