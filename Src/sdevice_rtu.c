#include "../Inc/ModbusSDevice/core_rtu.h"

#include <memory.h>

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(ModbusRtu, handle)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->Constant != NULL);
   SDeviceAssert(handle->IsInitialized == false);
   SDeviceAssert(handle->Constant->Common.ReadRegisters != NULL);
   SDeviceAssert(handle->Constant->Common.WriteRegisters != NULL);

   handle->IsInitialized = true;
}

__SDEVICE_SET_SETTING_DECLARATION(ModbusRtu, SlaveAddress, handle, value)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(value != NULL);

   __SDEVICE_HANDLE(ModbusRtu) *deviceHandle = handle;

   SDeviceAssert(deviceHandle->IsInitialized == true);

   __typeof__(deviceHandle->Settings.SlaveAddress) address;
   memcpy(&address, value, sizeof(deviceHandle->Settings.SlaveAddress));

   if(__MODBUS_RTU_IS_VALID_SLAVE_ADDRESS(address) != true)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_RUNTIME_ERROR_SLAVE_ADDRESS_INVALID_SET);
      return SDEVICE_OPERATION_STATUS_VALIDATION_ERROR;
   }

   deviceHandle->Settings.SlaveAddress = address;

   return SDEVICE_OPERATION_STATUS_OK;
}
