#include "../Inc/ModbusSDevice/core_tcp.h"

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(ModbusTcp, handle)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->Constant != NULL);
   SDeviceAssert(handle->IsInitialized == false);
   SDeviceAssert(handle->Constant->ModbusCommon.ReadRegistersFunction != NULL);
   SDeviceAssert(handle->Constant->ModbusCommon.WriteRegistersFunction != NULL);

   handle->IsInitialized = true;
}
