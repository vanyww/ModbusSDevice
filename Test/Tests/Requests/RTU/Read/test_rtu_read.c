#include "test_rtu_read.h"
#include "../../../../Device/test_device.h"
#include "../../../../Device/Mock/Errors/errors.h"
#include "../../../../Device/Mock/Functions/mock_functions.h"

#include <memory.h>

bool TestRtuReadOneRequest(void)
{
   uint8_t slaveAddress = 0xAA;
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusRtu))))
         SDEVICE_HANDLE(ModbusRtu) *handle = CreateModbusRtuSDevice();
   SDEVICE_SET_PROPERTY(ModbusRtu, SlaveAddress)(handle, &slaveAddress);

   MockReadRegisters[0] = 0xBBCC;

   const uint8_t request[] = { 0xAA, 0x03, 0x00, 0x00, 0x00, 0x01, 0x9D, 0xD1 };
   const uint8_t expectedReply[] = { 0xAA, 0x03, 0x02, 0xCC, 0xBB, 0x88, 0xEF };
   ModbusSDeviceRequest requestData = { .Data = request, .Size = sizeof(request) };
   ModbusSDeviceResponse responseData = { .Data = (uint8_t[MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE]){ } };

   if(ModbusRtuSDeviceTryProcessRequest(handle, &requestData, &responseData) != true)
      return false;

   if(AnyErrorDetected())
      return false;

   if(responseData.Size != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Data, responseData.Size) != 0)
      return false;

   return true;
}

bool TestRtuReadMultipleRequest(void)
{
   uint8_t slaveAddress = 0xAA;
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusRtu))))
         SDEVICE_HANDLE(ModbusRtu) *handle = CreateModbusRtuSDevice();
   SDEVICE_SET_PROPERTY(ModbusRtu, SlaveAddress)(handle, &slaveAddress);

   MockReadRegisters[0] = 0x2211;
   MockReadRegisters[1] = 0x4433;
   MockReadRegisters[2] = 0x6655;

   const uint8_t request[] = { 0xAA, 0x03, 0x00, 0x00, 0x00, 0x03, 0x1C, 0x10 };
   const uint8_t expectedReply[] = { 0xAA, 0x03, 0x06, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0xA1, 0x2F };
   ModbusSDeviceRequest requestData = { .Data = request, .Size = sizeof(request) };
   ModbusSDeviceResponse responseData = { .Data = (uint8_t[MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE]){ } };

   if(ModbusRtuSDeviceTryProcessRequest(handle, &requestData, &responseData) != true)
      return false;

   if(AnyErrorDetected())
      return false;

   if(responseData.Size != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Data, responseData.Size) != 0)
      return false;

   return true;
}

bool TestRtuReadTooManyRequest(void)
{
   uint8_t slaveAddress = 0xAA;
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusRtu))))
         SDEVICE_HANDLE(ModbusRtu) *handle = CreateModbusRtuSDevice();
   SDEVICE_SET_PROPERTY(ModbusRtu, SlaveAddress)(handle, &slaveAddress);

   const uint8_t request[] = { 0xAA, 0x03, 0x00, 0x00, 0x00, 0xFF, 0x1C, 0x51 };
   const uint8_t expectedReply[] = { 0xAA, 0x83, 0x03, 0x70, 0xD1 };
   ModbusSDeviceRequest requestData = { .Data = request, .Size = sizeof(request) };
   ModbusSDeviceResponse responseData = { .Data = (uint8_t[MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE]){ } };

   if(ModbusRtuSDeviceTryProcessRequest(handle, &requestData, &responseData) != true)
      return false;

   if(!AnyErrorDetected())
      return false;

   if(responseData.Size != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Data, responseData.Size) != 0)
      return false;

   return true;
}

bool TestRtuReadWithWrongSlaveAddressRequest(void)
{
   uint8_t slaveAddress = 0xAA;
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusRtu))))
         SDEVICE_HANDLE(ModbusRtu) *handle = CreateModbusRtuSDevice();
   SDEVICE_SET_PROPERTY(ModbusRtu, SlaveAddress)(handle, &slaveAddress);

   const uint8_t request[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A };
   ModbusSDeviceRequest requestData = { .Data = request, .Size = sizeof(request) };
   ModbusSDeviceResponse responseData = { .Data = (uint8_t[MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE]){ } };

   if(ModbusRtuSDeviceTryProcessRequest(handle, &requestData, &responseData) != false)
      return false;

   if(AnyErrorDetected())
      return false;

   return true;
}
