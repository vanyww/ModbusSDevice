#include "test_rtu_read.h"
#include "../../../../Mock/Errors/errors.h"
#include "../../../../Mock/SDevice/test_device.h"
#include "../../../../Mock/SDevice/Bindings/bindings.h"

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

   if(WAS_ANY_ERROR_THROWN())
      return false;

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, replyBuffer, replySize) != 0)
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

   if(WAS_ANY_ERROR_THROWN())
      return false;

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, replyBuffer, replySize) != 0)
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

   if(!WAS_ANY_ERROR_THROWN())
      return false;

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, replyBuffer, replySize) != 0)
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
   uint8_t replyBuffer[MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE];
   size_t replySize;

   ModbusRtuSDeviceInput input =
   {
      request,
      sizeof(request)
   };
   ModbusRtuSDeviceOutput output = { replyBuffer, &replySize };

   if(ModbusRtuSDeviceTryProcessRequest(handle, input, output) == true)
      return false;

   if(WAS_ANY_ERROR_THROWN())
      return false;

   return true;
}
