#include "test_rtu_write.h"
#include "../../../../Device/test_device.h"
#include "../../../../Device/Mock/Errors/errors.h"
#include "../../../../Device/Mock/Functions/mock_functions.h"

#include <memory.h>

bool TestRtuWriteOneRequest(void)
{
   uint8_t slaveAddress = 0xAA;
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusRtu))))
         SDEVICE_HANDLE(ModbusRtu) *handle = CreateModbusRtuSDevice();
   SDEVICE_SET_PROPERTY(ModbusRtu, SlaveAddress)(handle, &slaveAddress);

   const uint8_t request[] = { 0xAA, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x11, 0x22, 0xA1, 0x2E };
   const uint8_t expectedReply[] = { 0xAA, 0x10, 0x00, 0x00, 0x00, 0x01, 0x18, 0x12 };
   ModbusSDeviceRequest requestData = { .Data = request, .Size = sizeof(request) };
   ModbusSDeviceResponse responseData = { .Data = (uint8_t[MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE]){ } };

   if(ModbusRtuSDeviceTryProcessRequest(handle, &requestData, &responseData) != true)
      return false;

   if(responseData.Size != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Data, responseData.Size) != 0)
      return false;

   if(MockWriteRegisters[0] != 0x2211)
      return false;

   if(AnyErrorDetected())
      return false;

   return true;
}

bool TestRtuWriteMultipleRequest(void)
{
   uint8_t slaveAddress = 0xAA;
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusRtu))))
         SDEVICE_HANDLE(ModbusRtu) *handle = CreateModbusRtuSDevice();
   SDEVICE_SET_PROPERTY(ModbusRtu, SlaveAddress)(handle, &slaveAddress);

   const uint8_t request[] = { 0xAA, 0x10, 0x00, 0x00, 0x00, 0x02, 0x04, 0x11, 0x22, 0x33, 0x44, 0x65, 0x7C };
   const uint8_t expectedReply[] = { 0xAA, 0x10, 0x00, 0x00, 0x00, 0x02, 0x58, 0x13 };
   ModbusSDeviceRequest requestData = { .Data = request, .Size = sizeof(request) };
   ModbusSDeviceResponse responseData = { .Data = (uint8_t[MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE]){ } };

   if(ModbusRtuSDeviceTryProcessRequest(handle, &requestData, &responseData) != true)
      return false;

   if(responseData.Size != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Data, responseData.Size) != 0)
      return false;

   if(MockWriteRegisters[0] != 0x2211)
      return false;

   if(MockWriteRegisters[1] != 0x4433)
      return false;

   if(AnyErrorDetected())
      return false;

   return true;
}
