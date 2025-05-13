#include "ModbusSDevice/Tcp/public.h"
#include "../../Mock/SDevice/Bindings/io.h"

#include "SDeviceCore/common.h"

#include "unity_fixture.h"

#include <memory.h>

static SDEVICE_HANDLE(ModbusTcp) *Handle;

TEST_GROUP(ModbusTcpReadRequest);

TEST_SETUP(ModbusTcpReadRequest)
{
   SDEVICE_INIT_DATA(ModbusTcp) init =
   {
      .Base =
      {
         .ReadOperation  = MockReadOperation,
         .WriteOperation = MockWriteOperation
      },
   };

   Handle = SDEVICE_CREATE_HANDLE(ModbusTcp)(&init, NULL);

   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));
}

TEST_TEAR_DOWN(ModbusTcpReadRequest)
{
   SDEVICE_DISPOSE_HANDLE(ModbusTcp)(Handle);
}

TEST(ModbusTcpReadRequest, One)
{
   MockReadRegisters[0] = 0x2211;

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x01, 0x03, 0x02, 0x11, 0x22 };
   uint8_t replyBuffer[MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE];
   size_t packetSize;
   size_t replySize;

   ModbusTcpSDeviceInput input =
   {
      request + MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE,
      sizeof(request) - MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE
   };

   ModbusTcpSDeviceOutput output =
   {
      replyBuffer,
      &replySize
   };

   TEST_ASSERT(ModbusTcpSDeviceTryProcessMbapHeader(Handle, request, &packetSize));
   TEST_ASSERT(ModbusTcpSDeviceTryProcessRequest(Handle, input, output));
   TEST_ASSERT_EQUAL_UINT(sizeof(expectedReply), replySize);
   TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedReply, replyBuffer, LENGTHOF(expectedReply));
}

TEST(ModbusTcpReadRequest, Multiple)
{
   MockReadRegisters[0] = 0x2211;
   MockReadRegisters[1] = 0x4433;

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x02 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x01, 0x03, 0x04, 0x11, 0x22, 0x33, 0x44 };
   uint8_t replyBuffer[MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE];
   size_t packetSize;
   size_t replySize;

   ModbusTcpSDeviceInput input =
   {
      request + MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE,
      sizeof(request) - MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE
   };

   ModbusTcpSDeviceOutput output =
   {
      replyBuffer,
      &replySize
   };

   TEST_ASSERT(ModbusTcpSDeviceTryProcessMbapHeader(Handle, request, &packetSize));
   TEST_ASSERT(ModbusTcpSDeviceTryProcessRequest(Handle, input, output));
   TEST_ASSERT_EQUAL_UINT(sizeof(expectedReply), replySize);
   TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedReply, replyBuffer, LENGTHOF(expectedReply));
}

TEST(ModbusTcpReadRequest, TooMany)
{
   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0xFF };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x01, 0x83, 0x03 };
   uint8_t replyBuffer[MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE];
   size_t packetSize;
   size_t replySize;

   ModbusTcpSDeviceInput input =
   {
      request + MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE,
      sizeof(request) - MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE
   };

   ModbusTcpSDeviceOutput output =
   {
      replyBuffer,
      &replySize
   };

   TEST_ASSERT(ModbusTcpSDeviceTryProcessMbapHeader(Handle, request, &packetSize));
   TEST_ASSERT(ModbusTcpSDeviceTryProcessRequest(Handle, input, output));
   TEST_ASSERT_EQUAL_UINT(sizeof(expectedReply), replySize);
   TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedReply, replyBuffer, LENGTHOF(expectedReply));
}

TEST_GROUP_RUNNER(ModbusTcpReadRequest)
{
   RUN_TEST_CASE(ModbusTcpReadRequest, One);
   RUN_TEST_CASE(ModbusTcpReadRequest, Multiple);
   RUN_TEST_CASE(ModbusTcpReadRequest, TooMany);
}
