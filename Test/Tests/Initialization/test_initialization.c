#include "test_initialization.h"
#include "../../Device/test_device.h"
#include "../../Device/Mock/Assertation/mock_assert.h"

bool TestRtuHandleInitialization(void)
{
   __SDEVICE_HANDLE(Modbus) handle = { 0 };
   CreateModbusSDevice(MODBUS_SDEVICE_TYPE_RTU, &handle);

   if(handle.IsInitialized != true)
      return false;

   if(WasAssertFailed() == true)
      return false;

   return true;
}

bool TestTcpHandleInitialization(void)
{
   __SDEVICE_HANDLE(Modbus) handle = { 0 };
   CreateModbusSDevice(MODBUS_SDEVICE_TYPE_TCP, &handle);

   if(handle.IsInitialized != true)
      return false;

   if(WasAssertFailed() == true)
      return false;

   return true;
}
