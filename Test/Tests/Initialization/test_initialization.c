#include "test_initialization.h"
#include "../../Device/test_device.h"
#include "../../Device/Mock/Errors/errors.h"

bool TestRtuHandleInitialization(void)
{
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusRtu))))
         SDEVICE_HANDLE(ModbusRtu) *handle = CreateModbusRtuSDevice();

   if(AnyErrorDetected() == true)
      return false;

   return true;
}

bool TestTcpHandleInitialization(void)
{
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusTcp))))
         SDEVICE_HANDLE(ModbusTcp) *handle = CreateModbusTcpSDevice();

   if(AnyErrorDetected() == true)
      return false;

   return true;
}
