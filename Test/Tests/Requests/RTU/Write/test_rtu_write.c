#include "test_rtu_write.h"
#include "../../../../Device/test_device.h"
#include "../../../../Device/Mock/Assertation/mock_assert.h"
#include "../../../../Device/Mock/Functions/mock_functions.h"
#include "../../../../Device/Mock/RuntimeError/mock_handle_runtime_error.h"
#include "ModbusSDevice/rtu_defs.h"

#include <memory.h>

bool TestRtuWriteOneRequest(void)
{
   uint8_t slaveAddress = 0xAA;
   __SDEVICE_HANDLE(Modbus) handle = { 0 };
   CreateModbusSDevice(MODBUS_SDEVICE_TYPE_RTU, &handle);
   __SDEVICE_SET_SETTING(Modbus, SlaveAddress)(&handle, &slaveAddress);

   const uint8_t request[] = { 0xAA, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x11, 0x22, 0xA1, 0x2E };
   const uint8_t expectedReply[] = { 0xAA, 0x10, 0x00, 0x00, 0x00, 0x01, 0x18, 0x12 };
   ModbusSDeviceRequest requestData = { .Bytes = request, .BytesCount = sizeof(request) };
   ModbusSDeviceResponse responseData = { .Bytes = (uint8_t[__MODBUS_SDEVICE_RTU_MAX_MESSAGE_SIZE]){ } };

   if(ModbusSDeviceTryProcessRequest(&handle, &requestData, &responseData) != true)
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

bool TestRtuWriteMultipleRequest(void)
{
   uint8_t slaveAddress = 0xAA;
   __SDEVICE_HANDLE(Modbus) handle = { 0 };
   CreateModbusSDevice(MODBUS_SDEVICE_TYPE_RTU, &handle);
   __SDEVICE_SET_SETTING(Modbus, SlaveAddress)(&handle, &slaveAddress);

   const uint8_t request[] = { 0xAA, 0x10, 0x00, 0x00, 0x00, 0x02, 0x04, 0x11, 0x22, 0x33, 0x44, 0x65, 0x7C };
   const uint8_t expectedReply[] = { 0xAA, 0x10, 0x00, 0x00, 0x00, 0x02, 0x58, 0x13 };
   ModbusSDeviceRequest requestData = { .Bytes = request, .BytesCount = sizeof(request) };
   ModbusSDeviceResponse responseData = { .Bytes = (uint8_t[__MODBUS_SDEVICE_RTU_MAX_MESSAGE_SIZE]){ } };

   if(ModbusSDeviceTryProcessRequest(&handle, &requestData, &responseData) != true)
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
