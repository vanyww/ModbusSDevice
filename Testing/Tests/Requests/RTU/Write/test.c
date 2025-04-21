#include "../../../../Mock/SDevice/modbus_rtu.h"
#include "../../../../Mock/SDevice/Bindings/io.h"

#include "SDeviceCore/common.h"

#include "unity_fixture.h"

#include <memory.h>

#define _cleanup __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusRtu))))

TEST_GROUP(ModbusRtuWriteRequest);

TEST_SETUP(ModbusRtuWriteRequest) { }
TEST_TEAR_DOWN(ModbusRtuWriteRequest) { }

TEST(ModbusRtuWriteRequest, One)
{
   _cleanup SDEVICE_HANDLE(ModbusRtu) *handle = ModbusRtuSDeviceCreateInstance();

   uint8_t slaveAddress = 0xAA;
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusRtu, SlaveAddress)(handle, &slaveAddress);

   const uint8_t request[] = { slaveAddress, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x11, 0x22, 0xA1, 0x2E };
   const uint8_t expectedReply[] = { slaveAddress, 0x10, 0x00, 0x00, 0x00, 0x01, 0x18, 0x12 };
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
   TEST_ASSERT_EQUAL_UINT16(0x2211, MockWriteRegisters[0]);
}

TEST(ModbusRtuWriteRequest, Multiple)
{
   _cleanup SDEVICE_HANDLE(ModbusRtu) *handle = ModbusRtuSDeviceCreateInstance();

   uint8_t slaveAddress = 0xAA;
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusRtu, SlaveAddress)(handle, &slaveAddress);

   const uint8_t request[] = { slaveAddress, 0x10, 0x00, 0x00, 0x00, 0x02, 0x04, 0x11, 0x22, 0x33, 0x44, 0x65, 0x7C };
   const uint8_t expectedReply[] = { slaveAddress, 0x10, 0x00, 0x00, 0x00, 0x02, 0x58, 0x13 };
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
   TEST_ASSERT_EQUAL_UINT16(0x2211, MockWriteRegisters[0]);
   TEST_ASSERT_EQUAL_UINT16(0x4433, MockWriteRegisters[1]);
}

TEST_GROUP_RUNNER(ModbusRtuWriteRequest)
{
   RUN_TEST_CASE(ModbusRtuWriteRequest, One);
   RUN_TEST_CASE(ModbusRtuWriteRequest, Multiple);
}
