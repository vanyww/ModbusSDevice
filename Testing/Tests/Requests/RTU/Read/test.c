#include "../../../../Mock/SDevice/modbus_rtu.h"
#include "../../../../Mock/SDevice/Bindings/io.h"

#include "SDeviceCore/common.h"

#include "unity_fixture.h"

#include <memory.h>

#define _cleanup __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusRtu))))

TEST_GROUP(ModbusRtuReadRequest);

TEST_SETUP(ModbusRtuReadRequest) { }
TEST_TEAR_DOWN(ModbusRtuReadRequest) { }

TEST(ModbusRtuReadRequest, One)
{
   _cleanup SDEVICE_HANDLE(ModbusRtu) *handle = ModbusRtuSDeviceCreateInstance();

   uint8_t slaveAddress = 0xAA;
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusRtu, SlaveAddress)(handle, &slaveAddress);

   MockReadRegisters[0] = 0xBBCC;

   const uint8_t request[] = { slaveAddress, 0x03, 0x00, 0x00, 0x00, 0x01, 0x9D, 0xD1 };
   const uint8_t expectedReply[] = { slaveAddress, 0x03, 0x02, 0xCC, 0xBB, 0x88, 0xEF };
   uint8_t replyBuffer[MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE];
   size_t replySize;

   ModbusRtuSDeviceInput input =
   {
      request,
      sizeof(request)
   };

   ModbusRtuSDeviceOutput output =
   {
      replyBuffer,
      &replySize
   };

   TEST_ASSERT(ModbusRtuSDeviceTryProcessRequest(handle, input, output));
   TEST_ASSERT_EQUAL_UINT(sizeof(expectedReply), replySize);
   TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedReply, replyBuffer, LENGTHOF(expectedReply));
}

TEST(ModbusRtuReadRequest, Multiple)
{
   _cleanup SDEVICE_HANDLE(ModbusRtu) *handle = ModbusRtuSDeviceCreateInstance();

   uint8_t slaveAddress = 0xAA;
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusRtu, SlaveAddress)(handle, &slaveAddress);

   MockReadRegisters[0] = 0x2211;
   MockReadRegisters[1] = 0x4433;
   MockReadRegisters[2] = 0x6655;

   const uint8_t request[] = { slaveAddress, 0x03, 0x00, 0x00, 0x00, 0x03, 0x1C, 0x10 };
   const uint8_t expectedReply[] = { slaveAddress, 0x03, 0x06, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0xA1, 0x2F };
   uint8_t replyBuffer[MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE];
   size_t replySize;

   ModbusRtuSDeviceInput input =
   {
      request,
      sizeof(request)
   };

   ModbusRtuSDeviceOutput output =
   {
      replyBuffer,
      &replySize
   };

   TEST_ASSERT(ModbusRtuSDeviceTryProcessRequest(handle, input, output));
   TEST_ASSERT_EQUAL_UINT(sizeof(expectedReply), replySize);
   TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedReply, replyBuffer, LENGTHOF(expectedReply));
}

TEST(ModbusRtuReadRequest, TooMany)
{
   _cleanup SDEVICE_HANDLE(ModbusRtu) *handle = ModbusRtuSDeviceCreateInstance();

   uint8_t slaveAddress = 0xAA;
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusRtu, SlaveAddress)(handle, &slaveAddress);

   const uint8_t request[] = { slaveAddress, 0x03, 0x00, 0x00, 0x00, 0xFF, 0x1C, 0x51 };
   const uint8_t expectedReply[] = { slaveAddress, 0x83, 0x03, 0x70, 0xD1 };
   uint8_t replyBuffer[MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE];
   size_t replySize;

   ModbusRtuSDeviceInput input =
   {
      request,
      sizeof(request)
   };

   ModbusRtuSDeviceOutput output =
   {
      replyBuffer,
      &replySize
   };

   TEST_ASSERT(ModbusRtuSDeviceTryProcessRequest(handle, input, output));
   TEST_ASSERT_EQUAL_UINT(sizeof(expectedReply), replySize);
   TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedReply, replyBuffer, LENGTHOF(expectedReply));
}

TEST(ModbusRtuReadRequest, WrongSlaveAddress)
{
   _cleanup SDEVICE_HANDLE(ModbusRtu) *handle = ModbusRtuSDeviceCreateInstance();

   uint8_t slaveAddress = 0xAA;
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusRtu, SlaveAddress)(handle, &slaveAddress);

   const uint8_t request[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A };
   uint8_t replyBuffer[MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE];
   size_t replySize;

   ModbusRtuSDeviceInput input =
   {
      request,
      sizeof(request)
   };

   ModbusRtuSDeviceOutput output =
   {
      replyBuffer,
      &replySize
   };

   TEST_ASSERT(!ModbusRtuSDeviceTryProcessRequest(handle, input, output));
}

TEST_GROUP_RUNNER(ModbusRtuReadRequest)
{
   RUN_TEST_CASE(ModbusRtuReadRequest, One);
   RUN_TEST_CASE(ModbusRtuReadRequest, Multiple);
   RUN_TEST_CASE(ModbusRtuReadRequest, TooMany);
   RUN_TEST_CASE(ModbusRtuReadRequest, WrongSlaveAddress);
}
