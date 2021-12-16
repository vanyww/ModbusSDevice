#include "../Inc/ModbusSDevice/core.h"

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(Modbus, handle)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->Constant != NULL);
   SDeviceAssert(handle->IsInitialized == false);
   SDeviceAssert(handle->Constant->ReadRegistersFunction != NULL);
   SDeviceAssert(handle->Constant->WriteRegistersFunction != NULL);
   SDeviceAssert(__MODBUS_SDEVICE_IS_VALID_TYPE(handle->Constant->Type) == true);

   handle->IsInitialized = true;
}
