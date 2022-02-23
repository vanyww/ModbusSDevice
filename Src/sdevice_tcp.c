#include "../Inc/ModbusSDevice/core_tcp.h"

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(ModbusTcp, handle)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->IsInitialized == false);
   SDeviceAssert(handle->Init.Common.ReadRegisters != NULL);
   SDeviceAssert(handle->Init.Common.WriteRegisters != NULL);

   handle->IsInitialized = true;
}
