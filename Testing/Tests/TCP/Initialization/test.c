#include "ModbusSDevice/Tcp/public.h"
#include "../../Mock/SDevice/Bindings/io.h"

#include "unity_fixture.h"

static SDEVICE_HANDLE(ModbusTcp) *Handle;

TEST_GROUP(ModbusTcpInitialization);

TEST_SETUP(ModbusTcpInitialization)
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
}

TEST_TEAR_DOWN(ModbusTcpInitialization)
{
   SDEVICE_DISPOSE_HANDLE(ModbusTcp)(Handle);
}

TEST(ModbusTcpInitialization, Handle)
{
   TEST_ASSERT_NOT_NULL(Handle);
}

TEST_GROUP_RUNNER(ModbusTcpInitialization)
{
   RUN_TEST_CASE(ModbusTcpInitialization, Handle);
}
