#include "test_rtu_read.h"
#include "../../../../Device/test_device.h"
#include "../../../../Device/Mock/Assertation/mock_assert.h"
#include "../../../../Device/Mock/Functions/mock_functions.h"
#include "../../../../Device/Mock/RuntimeError/mock_handle_runtime_error.h"

#include <memory.h>

bool TestRtuReadOneRequest(void)
{
   uint8_t slaveAddress = 0xAA;
   __SDEVICE_HANDLE(ModbusRtu) handle = { 0 };
   CreateModbusRtuSDevice(&handle);
   __SDEVICE_SET_SETTING(ModbusRtu, SlaveAddress)(&handle, &slaveAddress);

   MockReadRegisters[0] = (ModbusRegister){ .AsValue = 0xCCBB };

   const uint8_t request[] = { 0xAA, 0x03, 0x00, 0x00, 0x00, 0x01, 0x9D, 0xD1 };
   const uint8_t expectedReply[] = { 0xAA, 0x03, 0x02, 0xCC, 0xBB, 0x88, 0xEF };
   ModbusRequest requestData = { .Bytes = request, .Size = sizeof(request) };
   ModbusResponse responseData = { .Bytes = (uint8_t[__MODBUS_RTU_MAX_MESSAGE_SIZE]){ } };

   if(ModbusRtuTryProcessRequest(&handle, &requestData, &responseData) != true)
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

bool TestRtuReadMultipleRequest(void)
{
   uint8_t slaveAddress = 0xAA;
   __SDEVICE_HANDLE(ModbusRtu) handle = { 0 };
   CreateModbusRtuSDevice(&handle);
   __SDEVICE_SET_SETTING(ModbusRtu, SlaveAddress)(&handle, &slaveAddress);

   MockReadRegisters[0] = (ModbusRegister){ .AsValue = 0x1122 };
   MockReadRegisters[1] = (ModbusRegister){ .AsValue = 0x3344 };
   MockReadRegisters[2] = (ModbusRegister){ .AsValue = 0x5566 };

   const uint8_t request[] = { 0xAA, 0x03, 0x00, 0x00, 0x00, 0x03, 0x1C, 0x10 };
   const uint8_t expectedReply[] = { 0xAA, 0x03, 0x06, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0xA1, 0x2F };
   ModbusRequest requestData = { .Bytes = request, .Size = sizeof(request) };
   ModbusResponse responseData = { .Bytes = (uint8_t[__MODBUS_RTU_MAX_MESSAGE_SIZE]){ } };

   if(ModbusRtuTryProcessRequest(&handle, &requestData, &responseData) != true)
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

bool TestRtuReadTooManyRequest(void)
{
   uint8_t slaveAddress = 0xAA;
   __SDEVICE_HANDLE(ModbusRtu) handle = { 0 };
   CreateModbusRtuSDevice(&handle);
   __SDEVICE_SET_SETTING(ModbusRtu, SlaveAddress)(&handle, &slaveAddress);

   const uint8_t request[] = { 0xAA, 0x03, 0x00, 0x00, 0x00, 0xFF, 0x1C, 0x51 };
   const uint8_t expectedReply[] = { 0xAA, 0x83, 0x03, 0x70, 0xD1 };
   ModbusRequest requestData = { .Bytes = request, .Size = sizeof(request) };
   ModbusResponse responseData = { .Bytes = (uint8_t[__MODBUS_RTU_MAX_MESSAGE_SIZE]){ } };

   if(ModbusRtuTryProcessRequest(&handle, &requestData, &responseData) != true)
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

bool TestRtuReadWithWrongSlaveAddressRequest(void)
{
   uint8_t slaveAddress = 0xAA;
   __SDEVICE_HANDLE(ModbusRtu) handle = { 0 };
   CreateModbusRtuSDevice(&handle);
   __SDEVICE_SET_SETTING(ModbusRtu, SlaveAddress)(&handle, &slaveAddress);

   const uint8_t request[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A };
   ModbusRequest requestData = { .Bytes = request, .Size = sizeof(request) };
   ModbusResponse responseData = { .Bytes = (uint8_t[__MODBUS_RTU_MAX_MESSAGE_SIZE]){ } };

   if(ModbusRtuTryProcessRequest(&handle, &requestData, &responseData) != false)
      return false;

   if(WasAssertFailed() != false)
      return false;

   if(WasRuntimeErrorRaised() == true)
      return false;

   return true;
}
