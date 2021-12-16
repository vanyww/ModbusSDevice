#include "ADU/RTU/adu.h"
#include "ADU/TCP/adu.h"

bool ModbusSDeviceTryProcessRequest(__SDEVICE_HANDLE(Modbus) *handle,
                                    ModbusSDeviceRequest *request,
                                    ModbusSDeviceResponse *response)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->IsInitialized == true);

   switch(handle->Constant->Type)
   {
      case MODBUS_SDEVICE_TYPE_RTU:
         return ModbusRtuTryProcessAdu(handle, request, response);

      case MODBUS_SDEVICE_TYPE_TCP:
         return ModbusTcpTryProcessAdu(handle, request, response);

      default:
         SDeviceAssert(false);
         return false;
   }
}
