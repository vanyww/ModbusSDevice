#include "ModbusSDevice/Udp/public.h"
#include "../../Mock/SDevice/Bindings/io.h"

#include "unity_fixture.h"

static SDEVICE_HANDLE(ModbusUdp) *Handle;

TEST_GROUP(ModbusUdpInitialization);

TEST_SETUP(ModbusUdpInitialization)
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
}

TEST_TEAR_DOWN(ModbusUdpInitialization)
{
   SDEVICE_DISPOSE_HANDLE(ModbusUdp)(Handle);
}

TEST(ModbusUdpInitialization, Handle)
{
   TEST_ASSERT_NOT_NULL(Handle);
}

TEST_GROUP_RUNNER(ModbusUdpInitialization)
{
   RUN_TEST_CASE(ModbusUdpInitialization, Handle);
}
