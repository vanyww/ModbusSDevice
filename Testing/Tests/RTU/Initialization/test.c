#include "ModbusSDevice/Rtu/public.h"
#include "../../Mock/SDevice/Bindings/io.h"

#include "unity_fixture.h"

static SDEVICE_HANDLE(ModbusRtu) *Handle;

TEST_GROUP(ModbusRtuInitialization);

TEST_SETUP(ModbusRtuInitialization)
{
   SDEVICE_INIT_DATA(ModbusRtu) init =
   {
      .Base =
      {
         .ReadOperation  = MockReadOperation,
         .WriteOperation = MockWriteOperation
      },
   };

   Handle = SDEVICE_CREATE_HANDLE(ModbusRtu)(&init, NULL);
}

TEST_TEAR_DOWN(ModbusRtuInitialization)
{
   SDEVICE_DISPOSE_HANDLE(ModbusRtu)(Handle);
}

TEST(ModbusRtuInitialization, Handle)
{
   TEST_ASSERT_NOT_NULL(Handle);
}

TEST_GROUP_RUNNER(ModbusRtuInitialization)
{
   RUN_TEST_CASE(ModbusRtuInitialization, Handle);
}
