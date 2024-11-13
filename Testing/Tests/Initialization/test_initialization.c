#include "test_initialization.h"
#include "../../Mock/Errors/errors.h"
#include "../../Mock/SDevice/test_device.h"

bool TestRtuHandleInitialization(void)
{
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusRtu))))
         SDEVICE_HANDLE(ModbusRtu) *handle = CreateModbusRtuSDevice();

   return !WAS_ANY_ERROR_THROWN();
}

bool TestTcpHandleInitialization(void)
{
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusTcp))))
         SDEVICE_HANDLE(ModbusTcp) *handle = CreateModbusTcpSDevice();

   return !WAS_ANY_ERROR_THROWN();
}
