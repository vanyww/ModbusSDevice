#include "unity_fixture.h"

static void RunAllTests(void)
{
   RUN_TEST_GROUP(ModbusRtuInitialization);
   RUN_TEST_GROUP(ModbusRtuReadRequest);
   RUN_TEST_GROUP(ModbusRtuWriteRequest);
   RUN_TEST_GROUP(ModbusTcpInitialization);
   RUN_TEST_GROUP(ModbusTcpReadRequest);
   RUN_TEST_GROUP(ModbusTcpWriteRequest);
   RUN_TEST_GROUP(ModbusUdpInitialization);
   RUN_TEST_GROUP(ModbusUdpReadRequest);
}

int main(int argc, char const *argv[])
{
   UnityMain(argc, argv, RunAllTests);
   return 0;
}
