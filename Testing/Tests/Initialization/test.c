#include "../../Mock/SDevice/modbus_rtu.h"
#include "../../Mock/SDevice/modbus_tcp.h"
#include "../../Mock/SDevice/modbus_udp.h"

#include "unity_fixture.h"

#define _cleanup(name) __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(name))))

TEST_GROUP(Initialization);

TEST_SETUP(Initialization) { }
TEST_TEAR_DOWN(Initialization) { }

TEST(Initialization, ModbusRtu)
{
   _cleanup(ModbusRtu)
         SDEVICE_HANDLE(ModbusRtu) *handle = ModbusRtuSDeviceCreateInstance();

   TEST_ASSERT_NOT_NULL(handle);
}

TEST(Initialization, ModbusTcp)
{
   _cleanup(ModbusTcp)
         SDEVICE_HANDLE(ModbusTcp) *handle = ModbusTcpSDeviceCreateInstance();

   TEST_ASSERT_NOT_NULL(handle);
}

TEST(Initialization, ModbusUdp)
{
   _cleanup(ModbusUdp)
         SDEVICE_HANDLE(ModbusUdp) *handle = ModbusUdpSDeviceCreateInstance();

   TEST_ASSERT_NOT_NULL(handle);
}

TEST_GROUP_RUNNER(Initialization)
{
   RUN_TEST_CASE(Initialization, ModbusRtu);
   RUN_TEST_CASE(Initialization, ModbusTcp);
   RUN_TEST_CASE(Initialization, ModbusUdp);
}
