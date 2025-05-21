#include "ModbusSDevice/Rtu/public.h"
#include "../../Mock/SDevice/Bindings/io.h"
#include "../../Mock/SDevice/Bindings/crc.h"

#include "SDeviceCore/common.h"

#include "unity_fixture.h"

#include <memory.h>

static SDEVICE_HANDLE(ModbusRtu) *Handle;

TEST_GROUP(ModbusRtuReadRequest);

TEST_SETUP(ModbusRtuReadRequest)
{
   SDEVICE_INIT_DATA(ModbusRtu) init =
   {
      .ComputeCrc16 = MockComputeCrc16,

      .Base =
      {
         .ReadOperation  = MockReadOperation,
         .WriteOperation = MockWriteOperation
      }
   };

   Handle = SDEVICE_CREATE_HANDLE(ModbusRtu)(&init, NULL);

   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));
}

TEST_TEAR_DOWN(ModbusRtuReadRequest)
{
   SDEVICE_DISPOSE_HANDLE(ModbusRtu)(Handle);
}

TEST(ModbusRtuReadRequest, One)
{
   uint8_t slaveAddress = 0xAA;
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusRtu, SlaveAddress)(Handle, &slaveAddress);

   MockReadRegisters[0] = 0xBBCC;

   const uint8_t request[] = { slaveAddress, 0x03, 0x00, 0x00, 0x00, 0x01, 0xEE, 0xEE };
   const uint8_t expectedReply[] = { slaveAddress, 0x03, 0x02, 0xCC, 0xBB, 0xEE, 0xEE };
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

   TEST_ASSERT(ModbusRtuSDeviceTryProcessRequest(Handle, input, output));
   TEST_ASSERT_EQUAL_UINT(sizeof(expectedReply), replySize);
   TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedReply, replyBuffer, LENGTHOF(expectedReply));
}

TEST(ModbusRtuReadRequest, Multiple)
{
   uint8_t slaveAddress = 0xAA;
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusRtu, SlaveAddress)(Handle, &slaveAddress);

   MockReadRegisters[0] = 0x2211;
   MockReadRegisters[1] = 0x4433;
   MockReadRegisters[2] = 0x6655;

   const uint8_t request[] = { slaveAddress, 0x03, 0x00, 0x00, 0x00, 0x03, 0xEE, 0xEE };
   const uint8_t expectedReply[] = { slaveAddress, 0x03, 0x06, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0xEE, 0xEE };
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

   TEST_ASSERT(ModbusRtuSDeviceTryProcessRequest(Handle, input, output));
   TEST_ASSERT_EQUAL_UINT(sizeof(expectedReply), replySize);
   TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedReply, replyBuffer, LENGTHOF(expectedReply));
}

TEST(ModbusRtuReadRequest, TooMany)
{
   uint8_t slaveAddress = 0xAA;
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusRtu, SlaveAddress)(Handle, &slaveAddress);

   const uint8_t request[] = { slaveAddress, 0x03, 0x00, 0x00, 0x00, 0xFF, 0xEE, 0xEE };
   const uint8_t expectedReply[] = { slaveAddress, 0x83, 0x03, 0xEE, 0xEE };
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

   TEST_ASSERT(ModbusRtuSDeviceTryProcessRequest(Handle, input, output));
   TEST_ASSERT_EQUAL_UINT(sizeof(expectedReply), replySize);
   TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedReply, replyBuffer, LENGTHOF(expectedReply));
}

TEST(ModbusRtuReadRequest, WrongSlaveAddress)
{
   uint8_t slaveAddress = 0xAA;
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusRtu, SlaveAddress)(Handle, &slaveAddress);

   const uint8_t request[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0xEE, 0xEE };
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

   TEST_ASSERT(!ModbusRtuSDeviceTryProcessRequest(Handle, input, output));
}

TEST_GROUP_RUNNER(ModbusRtuReadRequest)
{
   RUN_TEST_CASE(ModbusRtuReadRequest, One);
   RUN_TEST_CASE(ModbusRtuReadRequest, Multiple);
   RUN_TEST_CASE(ModbusRtuReadRequest, TooMany);
   RUN_TEST_CASE(ModbusRtuReadRequest, WrongSlaveAddress);
}
