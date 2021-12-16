#include "test_tcp_read.h"
#include "../../../../Device/test_device.h"
#include "../../../../Device/Mock/Assertation/mock_assert.h"
#include "../../../../Device/Mock/Functions/mock_functions.h"
#include "../../../../Device/Mock/RuntimeError/mock_handle_runtime_error.h"
#include "ModbusSDevice/rtu_defs.h"

#include <memory.h>

bool TestTcpReadOneRequest(void)
{
   __SDEVICE_HANDLE(Modbus) handle = { 0 };
   CreateModbusSDevice(MODBUS_SDEVICE_TYPE_TCP, &handle);

   MockReadRegisters[0] = (ModbusSDeviceRegister){ .AsValue = 0x1122 };

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x01, 0x03, 0x02, 0x11, 0x22 };
   ModbusSDeviceRequest requestData = { .Bytes = request, .BytesCount = sizeof(request) };
   ModbusSDeviceResponse responseData = { .Bytes = (uint8_t[__MODBUS_SDEVICE_RTU_MAX_MESSAGE_SIZE]){ } };

   if(ModbusSDeviceTryProcessRequest(&handle, &requestData, &responseData) != true)
      return false;

   if(WasAssertFailed() != false)
      return false;

   if(WasRuntimeErrorRaised() == true)
      return false;

   if(responseData.BytesCount != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Bytes, responseData.BytesCount) != 0)
      return false;

   return true;
}

bool TestTcpReadMultipleRequest(void)
{
   __SDEVICE_HANDLE(Modbus) handle = { 0 };
   CreateModbusSDevice(MODBUS_SDEVICE_TYPE_TCP, &handle);

   MockReadRegisters[0] = (ModbusSDeviceRegister){ .AsValue = 0x1122 };
   MockReadRegisters[1] = (ModbusSDeviceRegister){ .AsValue = 0x3344 };

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x02 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x01, 0x03, 0x04, 0x11, 0x22, 0x33, 0x44 };
   ModbusSDeviceRequest requestData = { .Bytes = request, .BytesCount = sizeof(request) };
   ModbusSDeviceResponse responseData = { .Bytes = (uint8_t[__MODBUS_SDEVICE_RTU_MAX_MESSAGE_SIZE]){ } };

   if(ModbusSDeviceTryProcessRequest(&handle, &requestData, &responseData) != true)
      return false;

   if(WasAssertFailed() != false)
      return false;

   if(WasRuntimeErrorRaised() == true)
      return false;

   if(responseData.BytesCount != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Bytes, responseData.BytesCount) != 0)
      return false;

   return true;
}

bool TestTcpReadTooManyRequest(void)
{
   __SDEVICE_HANDLE(Modbus) handle = { 0 };
   CreateModbusSDevice(MODBUS_SDEVICE_TYPE_TCP, &handle);

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0xFF };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x01, 0x83, 0x03 };
   ModbusSDeviceRequest requestData = { .Bytes = request, .BytesCount = sizeof(request) };
   ModbusSDeviceResponse responseData = { .Bytes = (uint8_t[__MODBUS_SDEVICE_RTU_MAX_MESSAGE_SIZE]){ } };

   if(ModbusSDeviceTryProcessRequest(&handle, &requestData, &responseData) != true)
      return false;

   if(WasAssertFailed() != false)
      return false;

   if(WasRuntimeErrorRaised() != true)
      return false;

   if(responseData.BytesCount != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Bytes, responseData.BytesCount) != 0)
      return false;

   return true;
}
