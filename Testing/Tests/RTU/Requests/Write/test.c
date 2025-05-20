#include "ModbusSDevice/Rtu/public.h"
#include "../../Mock/SDevice/Bindings/io.h"
#include "../../Mock/SDevice/Bindings/crc.h"

#include "SDeviceCore/common.h"

#include "unity_fixture.h"

#include <memory.h>

static SDEVICE_HANDLE(ModbusRtu) *Handle;

TEST_GROUP(ModbusRtuWriteRequest);

TEST_SETUP(ModbusRtuWriteRequest)
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

TEST_TEAR_DOWN(ModbusRtuWriteRequest)
{
   SDEVICE_DISPOSE_HANDLE(ModbusRtu)(Handle);
}

TEST(ModbusRtuWriteRequest, One)
{
   uint8_t slaveAddress = 0xAA;
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusRtu, SlaveAddress)(Handle, &slaveAddress);

   const uint8_t request[] = { slaveAddress, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x11, 0x22, 0xEE, 0xEE };
   const uint8_t expectedReply[] = { slaveAddress, 0x10, 0x00, 0x00, 0x00, 0x01, 0xEE, 0xEE };
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
   TEST_ASSERT_EQUAL_UINT16(0x2211, MockWriteRegisters[0]);
}

TEST(ModbusRtuWriteRequest, Multiple)
{
   uint8_t slaveAddress = 0xAA;
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusRtu, SlaveAddress)(Handle, &slaveAddress);

   const uint8_t request[] = { slaveAddress, 0x10, 0x00, 0x00, 0x00, 0x02, 0x04, 0x11, 0x22, 0x33, 0x44, 0xEE, 0xEE };
   const uint8_t expectedReply[] = { slaveAddress, 0x10, 0x00, 0x00, 0x00, 0x02, 0xEE, 0xEE };
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
   TEST_ASSERT_EQUAL_UINT16(0x2211, MockWriteRegisters[0]);
   TEST_ASSERT_EQUAL_UINT16(0x4433, MockWriteRegisters[1]);
}

TEST_GROUP_RUNNER(ModbusRtuWriteRequest)
{
   RUN_TEST_CASE(ModbusRtuWriteRequest, One);
   RUN_TEST_CASE(ModbusRtuWriteRequest, Multiple);
}
