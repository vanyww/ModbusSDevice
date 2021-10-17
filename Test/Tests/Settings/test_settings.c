#include "test_settings.h"
#include "../../Device/test_device.h"
#include "../../Device/Mock/Assertation/mock_assert.h"
#include "ModbusSDevice/RTU/rtu_defs.h"

bool TestSlaveAddressSettingSet(void)
{
   __SDEVICE_HANDLE(Modbus) handle = CreateModbusSDevice(MODBUS_SDEVICE_TYPE_RTU);

   for(size_t i = 0; i <= UINT8_MAX; i++)
   {
      uint8_t address = i;
      SDeviceSettingSetStatus status = __SDEVICE_SET_SETTING(Modbus, SlaveAddress)(&handle, &address);

      if(WasAssertFailed() == true)
         return false;

      if(status == SDEVICE_SETTING_SET_STATUS_SET_ERROR)
         return false;

      if(address == 0 || address > __MODBUS_SDEVICE_MAX_VALID_SLAVE_ADDRESS)
      {
         if(status != SDEVICE_SETTING_SET_STATUS_VALIDATION_ERROR)
            return false;
      }
      else
      {
         if(handle.Settings.Rtu.SlaveAddress != i)
            return false;
      }
   }

   return true;
}
