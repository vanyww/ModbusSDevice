#include "../Inc/ModbusSDevice/core_tcp.h"

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(ModbusTcp, handle)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->Constant != NULL);
   SDeviceAssert(handle->IsInitialized == false);
   SDeviceAssert(handle->Constant->Common.ReadRegisters != NULL);
   SDeviceAssert(handle->Constant->Common.WriteRegisters != NULL);

   handle->IsInitialized = true;
}
