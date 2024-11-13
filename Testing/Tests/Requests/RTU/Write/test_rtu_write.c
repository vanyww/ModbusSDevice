#include "test_rtu_write.h"
#include "../../../../Mock/Errors/errors.h"
#include "../../../../Mock/SDevice/test_device.h"
#include "../../../../Mock/SDevice/Bindings/bindings.h"

#include <memory.h>

bool TestRtuWriteOneRequest(void)
{
   uint8_t slaveAddress = 0xAA;
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusRtu))))
         SDEVICE_HANDLE(ModbusRtu) *handle = CreateModbusRtuSDevice();
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusRtu, SlaveAddress)(handle, &slaveAddress);

   const uint8_t request[] = { 0xAA, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x11, 0x22, 0xA1, 0x2E };
   const uint8_t expectedReply[] = { 0xAA, 0x10, 0x00, 0x00, 0x00, 0x01, 0x18, 0x12 };
   uint8_t replyBuffer[MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE];
   size_t replySize;

   ModbusRtuSDeviceInput input =
   {
      request,
      sizeof(request)
   };
   ModbusRtuSDeviceOutput output = { replyBuffer, &replySize };

   if(ModbusRtuSDeviceTryProcessRequest(handle, input, output) != true)
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

bool TestRtuWriteMultipleRequest(void)
{
   uint8_t slaveAddress = 0xAA;
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusRtu))))
         SDEVICE_HANDLE(ModbusRtu) *handle = CreateModbusRtuSDevice();
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusRtu, SlaveAddress)(handle, &slaveAddress);

   const uint8_t request[] = { 0xAA, 0x10, 0x00, 0x00, 0x00, 0x02, 0x04, 0x11, 0x22, 0x33, 0x44, 0x65, 0x7C };
   const uint8_t expectedReply[] = { 0xAA, 0x10, 0x00, 0x00, 0x00, 0x02, 0x58, 0x13 };
   uint8_t replyBuffer[MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE];
   size_t replySize;

   ModbusRtuSDeviceInput input =
   {
      request,
      sizeof(request)
   };
   ModbusRtuSDeviceOutput output = { replyBuffer, &replySize };

   if(ModbusRtuSDeviceTryProcessRequest(handle, input, output) != true)
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
