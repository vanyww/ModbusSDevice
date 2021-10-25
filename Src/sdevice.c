#include "Protocols/RTU/adu.h"
#include "Protocols/TCP/adu.h"

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(Modbus, handle)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->Constant != NULL);
   SDeviceAssert(handle->IsInitialized == false);
   SDeviceAssert(handle->Constant->ReceiveBuffer != NULL);
   SDeviceAssert(handle->Constant->TransmitBuffer != NULL);
   SDeviceAssert(handle->Constant->ReadRegistersFunction != NULL);
   SDeviceAssert(handle->Constant->WriteRegistersFunction != NULL);
   SDeviceAssert(__MODBUS_SDEVICE_IS_VALID_TYPE_VALUE(handle->Constant->Type) == true);

   switch(handle->Constant->Type)
   {
      case MODBUS_SDEVICE_TYPE_RTU:
         handle->Dynamic.ReceiveBufferPdu = &((__MODBUS_RTU_EMPTY_ADU *)handle->Constant->ReceiveBuffer)->Pdu;
         handle->Dynamic.ReceiveBufferFunctionSpecificData =
                  ((__MODBUS_RTU_EMPTY_ADU *)handle->Constant->ReceiveBuffer)->Pdu.FunctionSpecificData;

         handle->Dynamic.TransmitBufferPdu = &((__MODBUS_RTU_EMPTY_ADU *)handle->Constant->TransmitBuffer)->Pdu;
         handle->Dynamic.TransmitBufferFunctionSpecificData =
                  ((__MODBUS_RTU_EMPTY_ADU *)handle->Constant->TransmitBuffer)->Pdu.FunctionSpecificData;
         break;

      case MODBUS_SDEVICE_TYPE_TCP:
         handle->Dynamic.ReceiveBufferPdu= &((ModbusTcpAduData *)handle->Constant->ReceiveBuffer)->Pdu;
         handle->Dynamic.ReceiveBufferFunctionSpecificData =
                  ((ModbusTcpAduData *)handle->Constant->ReceiveBuffer)->Pdu.FunctionSpecificData;

         handle->Dynamic.TransmitBufferPdu= &((ModbusTcpAduData *)handle->Constant->TransmitBuffer)->Pdu;
         handle->Dynamic.TransmitBufferFunctionSpecificData =
                  ((ModbusTcpAduData *)handle->Constant->TransmitBuffer)->Pdu.FunctionSpecificData;
         break;

      default:
         return;
   }

   handle->IsInitialized = true;
}
