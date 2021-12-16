#include "adu.h"
#include "CRC/modbus_crc.h"
#include "../../PDU/pdu.h"
#include "../../../Inc/ModbusSDevice/rtu_defs.h"

#define __MODBUS_RTU_EMPTY_ADU_SIZE (sizeof(__MODBUS_RTU_ADU(0)))
#define __MODBUS_RTU_ADU(pdu_size)                                                                                     \
struct __attribute__((packed))                                                                                         \
{                                                                                                                      \
   uint8_t SlaveAddress;                                                                                               \
   uint8_t PduBytes[(pdu_size)];                                                                                       \
   ModbusRtuCrcType Crc16;                                                                                             \
}

bool ModbusRtuTryProcessAdu(__SDEVICE_HANDLE(Modbus) *handle,
                            ModbusSDeviceRequest *request,
                            ModbusSDeviceResponse *response)
{
   if(request->BytesCount < __MODBUS_RTU_EMPTY_ADU_SIZE ||
      request->BytesCount > __MODBUS_SDEVICE_RTU_MAX_MESSAGE_SIZE)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REQUEST_SIZE_ERROR);
      return false;
   }

   const __MODBUS_RTU_ADU(request->BytesCount - __MODBUS_RTU_EMPTY_ADU_SIZE) *requestAdu = request->Bytes;
   ModbusSDeviceRtuRequestType requestType;

   if(requestAdu->SlaveAddress != handle->Settings.Rtu.SlaveAddress)
   {
      if(requestAdu->SlaveAddress != __MODBUS_SDEVICE_BROADCAST_REQUEST_SLAVE_ADDRESS)
         return false;

      requestType = MODBUS_SDEVICE_RTU_REQUEST_TYPE_BROADCAST;
   }
   else
   {
      requestType = MODBUS_SDEVICE_RTU_REQUEST_TYPE_NORMAL;
   }

   if(requestAdu->Crc16 != ModbusRtuCrcCompute(request->Bytes, request->BytesCount - sizeof(ModbusRtuCrcType)))
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REQUEST_CRC_ERROR);
      return false;
   }

   ModbusSDeviceRequest requestPdu =
   {
      .Bytes = &requestAdu->PduBytes,
      .BytesCount = sizeof(requestAdu->PduBytes)
   };
   ModbusSDeviceResponse responsePdu =
   {
      .Bytes = &((__MODBUS_RTU_ADU(0) *)response->Bytes)->PduBytes
   };
   ModbusProcessingParameters processingParameters =
   {
      .RequestContext = &(ModbusSDeviceRtuRequestData){ .RequestType = requestType }
   };

   if(ModbusProcessPdu(handle, processingParameters, &requestPdu, &responsePdu) != true)
      return false;

   __MODBUS_RTU_ADU(responsePdu.BytesCount) *responseAdu = response->Bytes;

   if(requestType == MODBUS_SDEVICE_RTU_REQUEST_TYPE_BROADCAST)
   {
      response->BytesCount = 0;
      return true;
   }

   responseAdu->SlaveAddress = handle->Settings.Rtu.SlaveAddress;
   responseAdu->Crc16 = ModbusRtuCrcCompute(response->Bytes, sizeof(*responseAdu) - sizeof(ModbusRtuCrcType));

   response->BytesCount = sizeof(*responseAdu);
   return true;
}
