#include "ModbusSDevice/Udp/public.h"
#include "../../Mock/SDevice/Bindings/io.h"

#include "SDeviceCore/common.h"

#include "unity_fixture.h"

#include <memory.h>

static SDEVICE_HANDLE(ModbusUdp) *Handle;

TEST_GROUP(ModbusUdpReadRequest);

TEST_SETUP(ModbusUdpReadRequest)
{
   SDEVICE_INIT_DATA(ModbusUdp) init =
   {
      .Base =
      {
         .ReadOperation  = MockReadOperation,
         .WriteOperation = MockWriteOperation
      },
   };

   Handle = SDEVICE_CREATE_HANDLE(ModbusUdp)(&init, NULL);

   memset(MockReadRegisters, 0, sizeof(MockReadRegisters));
   memset(MockWriteRegisters, 0, sizeof(MockWriteRegisters));
}

TEST_TEAR_DOWN(ModbusUdpReadRequest)
{
   SDEVICE_DISPOSE_HANDLE(ModbusUdp)(Handle);
}

TEST(ModbusUdpReadRequest, One)
{
   const uint8_t btuAddress[] = { 1, 2, 3, 4, 5, 6 };
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusUdp, BtuAddress)(Handle, btuAddress);

   MockReadRegisters[0] = 0x2211;

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x01, 0x03, 0x02, 0x11, 0x22 };
   uint8_t replyBuffer[MODBUS_UDP_SDEVICE_MAX_MESSAGE_SIZE];
   size_t replySize;

   ModbusUdpSDeviceInput input = { request, sizeof(request) };
   ModbusUdpSDeviceOutput output = { replyBuffer, &replySize };

   TEST_ASSERT(ModbusUdpSDeviceTryProcessRequest(Handle, input, output));
   TEST_ASSERT_EQUAL_UINT(sizeof(expectedReply), replySize);
   TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedReply, replyBuffer, LENGTHOF(expectedReply));
}

TEST(ModbusUdpReadRequest, Multiple)
{
   const uint8_t btuAddress[] = { 1, 2, 3, 4, 5, 6 };
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusUdp, BtuAddress)(Handle, btuAddress);

   MockReadRegisters[0] = 0x2211;
   MockReadRegisters[1] = 0x4433;

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x02 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x01, 0x03, 0x04, 0x11, 0x22, 0x33, 0x44 };
   uint8_t replyBuffer[MODBUS_UDP_SDEVICE_MAX_MESSAGE_SIZE];
   size_t replySize;

   ModbusUdpSDeviceInput input = { request, sizeof(request) };
   ModbusUdpSDeviceOutput output = { replyBuffer, &replySize };

   TEST_ASSERT(ModbusUdpSDeviceTryProcessRequest(Handle, input, output));
   TEST_ASSERT_EQUAL_UINT(sizeof(expectedReply), replySize);
   TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedReply, replyBuffer, LENGTHOF(expectedReply));
}

TEST(ModbusUdpReadRequest, OneBtu)
{
   const uint8_t btuAddress[] = { 1, 2, 3, 4, 5, 6 };
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusUdp, BtuAddress)(Handle, btuAddress);

   MockReadRegisters[0] = 0x2211;

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x74, 0x00, 0x0C, 0x01,
                               1,    2,    3,    4,    5,    6,
                               0x03, 0x00, 0x00, 0x00, 0x01 };

   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x74, 0x00, 0x0B, 0x01,
                                     1,    2,    3,    4,    5,    6,
                                     0x03, 0x02, 0x11, 0x22 };

   uint8_t replyBuffer[MODBUS_UDP_SDEVICE_MAX_MESSAGE_SIZE];
   size_t replySize;

   ModbusUdpSDeviceInput input = { request, sizeof(request) };
   ModbusUdpSDeviceOutput output = { replyBuffer, &replySize };

   TEST_ASSERT(ModbusUdpSDeviceTryProcessRequest(Handle, input, output));
   TEST_ASSERT_EQUAL_UINT(sizeof(expectedReply), replySize);
   TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedReply, replyBuffer, LENGTHOF(expectedReply));
}

TEST_GROUP_RUNNER(ModbusUdpReadRequest)
{
   RUN_TEST_CASE(ModbusUdpReadRequest, One);
   RUN_TEST_CASE(ModbusUdpReadRequest, Multiple);
   RUN_TEST_CASE(ModbusUdpReadRequest, OneBtu);
}
