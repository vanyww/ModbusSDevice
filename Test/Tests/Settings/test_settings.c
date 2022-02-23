#include "test_settings.h"
#include "../../Device/test_device.h"
#include "../../Device/Mock/Assertation/mock_assert.h"
#include "../../Device/Mock/RuntimeError/mock_handle_runtime_error.h"

bool TestSlaveAddressSettingSet(void)
{
   __SDEVICE_HANDLE(ModbusRtu) handle =  CreateModbusRtuSDevice();

   for(size_t i = 0; i <= UINT8_MAX; i++)
   {
      uint8_t address = i;
      SDeviceOperationStatus status = __SDEVICE_SET_PARAMETER(ModbusRtu, SlaveAddress)(&handle, &address);

      if(WasAssertFailed() == true)
         return false;

      if(status == SDEVICE_OPERATION_STATUS_PROCESSING_ERROR)
         return false;

      if(address == 0 || address > __MODBUS_RTU_MAX_VALID_SLAVE_ADDRESS)
      {
         if(status != SDEVICE_OPERATION_STATUS_VALIDATION_ERROR || WasRuntimeErrorRaised() != true)
            return false;
      }
      else
      {
         if(handle.Runtime.SlaveAddress != i)
            return false;
      }

      if(WasRuntimeErrorRaised() == true)
         return false;
   }

   return true;
}
