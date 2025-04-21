#include "unity_fixture.h"

static void RunAllTests(void)
{
   RUN_TEST_GROUP(Initialization);
   RUN_TEST_GROUP(ModbusRtuReadRequest);
   RUN_TEST_GROUP(ModbusRtuWriteRequest);
   RUN_TEST_GROUP(ModbusTcpReadRequest);
   RUN_TEST_GROUP(ModbusTcpWriteRequest);
   RUN_TEST_GROUP(ModbusUdpReadRequest);
}

int main(int argc, char const *argv[])
{
   UnityMain(argc, argv, RunAllTests);
   return 0;
}
