#include "Protocols/RTU/adu.h"
#include "Protocols/TCP/adu.h"

size_t ModbusSDeviceProcessRequest(__SDEVICE_HANDLE(Modbus) *handle, size_t requestSize)
{
   SDeviceAssert(handle != NULL);

   switch(handle->Constant->Type)
   {
      case MODBUS_SDEVICE_TYPE_RTU:
         return ModbusRtuProcessAdu(handle, requestSize);

      case MODBUS_SDEVICE_TYPE_TCP:
         return ModbusTcpProcessAdu(handle, requestSize);

      default:
         return 0;
   }
}
