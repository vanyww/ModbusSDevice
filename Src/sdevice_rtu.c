#include "../Inc/ModbusSDevice/RTU/rtu_defs.h"
#include "../Inc/ModbusSDevice/core.h"

#include <memory.h>

__SDEVICE_SET_SETTING_DECLARATION(Modbus, SlaveAddress, handle, value)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(value != NULL);

   __SDEVICE_HANDLE(Modbus) *deviceHandle = handle;

   SDeviceAssert(deviceHandle->Constant->Type == MODBUS_SDEVICE_TYPE_RTU);
   SDeviceAssert(deviceHandle->IsInitialized == true);

   __typeof__(deviceHandle->Settings.Rtu.SlaveAddress) address;
   memcpy(&address, value, sizeof(deviceHandle->Settings.Rtu.SlaveAddress));

   if(__MODBUS_RTU_IS_VALID_SLAVE_ADDRESS(address) != true)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_SLAVE_ADDRESS_SETTING_VALIDATION_ERROR);
      return SDEVICE_SETTING_SET_STATUS_VALIDATION_ERROR;
   }

   deviceHandle->Settings.Rtu.SlaveAddress = address;

   return SDEVICE_SETTING_SET_STATUS_OK;
}
