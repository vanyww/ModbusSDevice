#include "../Inc/ModbusSDevice/core_rtu.h"

#include <memory.h>

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(ModbusRtu, handle)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->IsInitialized == false);
   SDeviceAssert(handle->Init.Common.ReadRegisters != NULL);
   SDeviceAssert(handle->Init.Common.WriteRegisters != NULL);

   handle->IsInitialized = true;
}

/* Slave address ******************************************************************************************************/

__SDEVICE_SET_PARAMETER_DECLARATION(ModbusRtu, SlaveAddress, handle, value)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(value != NULL);
   SDeviceAssert(((SDeviceCommonHandle *)handle)->IsInitialized == true);

   __SDEVICE_HANDLE(ModbusRtu) *device = handle;
   __typeof__(device->Runtime.SlaveAddress) address;
   memcpy(&address, value, sizeof(device->Runtime.SlaveAddress));

   if(__MODBUS_RTU_IS_VALID_SLAVE_ADDRESS(address) != true)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_RUNTIME_ERROR_SLAVE_ADDRESS_INVALID_SET);
      return SDEVICE_OPERATION_STATUS_VALIDATION_ERROR;
   }

   device->Runtime.SlaveAddress = address;

   return SDEVICE_OPERATION_STATUS_OK;
}

__SDEVICE_GET_PARAMETER_DECLARATION(ModbusRtu, SlaveAddress, handle, value)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(value != NULL);
   SDeviceAssert(((SDeviceCommonHandle *)handle)->IsInitialized == true);

   __SDEVICE_HANDLE(ModbusRtu) *device = handle;
   memcpy(value, &device->Runtime.SlaveAddress, sizeof(device->Runtime.SlaveAddress));

   return SDEVICE_OPERATION_STATUS_OK;
}

/**********************************************************************************************************************/
