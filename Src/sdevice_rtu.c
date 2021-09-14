#include "../Inc/ModbusSDevice/RTU/rtu_defs.h"
#include "../Inc/ModbusSDevice/core.h"

__SDEVICE_SET_SETTING_DECLARATION(Modbus, SlaveAddress, handle, value)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(value != NULL);

   __SDEVICE_HANDLE(Modbus) *deviceHandle = handle;

   SDeviceAssert(deviceHandle->Constant->Type == MODBUS_SDEVICE_TYPE_RTU);
   SDeviceAssert(deviceHandle->Settings != NULL);

   uint8_t address = *(uint8_t *)value;

   if(__MODBUS_RTU_IS_VALID_SLAVE_ADDRESS(address) == true)
      return SDEVICE_SETTING_SET_STATUS_VALIDATION_ERROR;

   deviceHandle->Settings->Rtu.SlaveAddress = address;

   return SDEVICE_SETTING_SET_STATUS_OK;
}
