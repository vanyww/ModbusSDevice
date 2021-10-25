#include "adu.h"
#include "../../../Inc/ModbusSDevice/RTU/rtu_defs.h"

size_t ModbusRtuProcessAdu(__SDEVICE_HANDLE(Modbus) *handle, size_t requestSize)
{
   if(requestSize < sizeof(__MODBUS_RTU_EMPTY_ADU) || requestSize > __MODBUS_SDEVICE_RTU_MAX_MESSAGE_SIZE)
      return 0;

   __MODBUS_RTU_ADU_STRUCT_DECLARATION(requestSize - __MODBUS_RTU_EMPTY_ADU_SIZE,) *requestAdu =
            handle->Constant->ReceiveBuffer;

   ModbusSDeviceRtuRequestType requestType;

   if(requestAdu->SlaveAddress != handle->Settings.Rtu.SlaveAddress)
   {
      if(requestAdu->SlaveAddress != __MODBUS_SDEVICE_BROADCAST_REQUEST_SLAVE_ADDRESS)
         return 0;

      requestType = MODBUS_SDEVICE_RTU_REQUEST_TYPE_BROADCAST;
   }
   else
   {
      requestType = MODBUS_SDEVICE_RTU_REQUEST_TYPE_NORMAL;
   }

   if(requestAdu->Crc16 != ModbusRtuCrcCompute(requestAdu, sizeof(*requestAdu) - sizeof(ModbusRtuCrcType)))
      return 0;

   size_t replyPduSize = ModbusProcessPdu(handle,
                                          &(ModbusCommonRequestProcessingData)
                                          {
                                             .PduSize = sizeof(requestAdu->PduBytes),
                                             .RequestParameters = &(ModbusSDeviceRtuRequestData)
                                             {
                                                .ModbusSDeviceRtuRequestType = requestType
                                             }
                                          });

   if(requestType == MODBUS_SDEVICE_RTU_REQUEST_TYPE_BROADCAST)
      return 0;

   __MODBUS_RTU_ADU_STRUCT_DECLARATION(replyPduSize,) *replyAdu = handle->Constant->TransmitBuffer;

   replyAdu->SlaveAddress = handle->Settings.Rtu.SlaveAddress;
   replyAdu->Crc16 = ModbusRtuCrcCompute(replyAdu, sizeof(*replyAdu) - sizeof(ModbusRtuCrcType));

   return sizeof(*replyAdu);
}
