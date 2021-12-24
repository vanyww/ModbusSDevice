#include "../Inc/ModbusSDevice/core_rtu.h"
#include "CRC/modbus_crc.h"
#include "PDU/pdu.h"

#define __MODBUS_RTU_EMPTY_ADU_SIZE (sizeof(__MODBUS_RTU_ADU(0)))
#define __MODBUS_RTU_ADU(pdu_size)                                                                                     \
struct __attribute__((packed))                                                                                         \
{                                                                                                                      \
   uint8_t SlaveAddress;                                                                                               \
   uint8_t PduBytes[(pdu_size)];                                                                                       \
   ModbusRtuCrcType Crc16;                                                                                             \
}

bool ModbusRtuSDeviceTryProcessRequest(__SDEVICE_HANDLE(ModbusRtu) *handle,
                                       ModbusSDeviceRequest *request,
                                       ModbusSDeviceResponse *response)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(request != NULL);
   SDeviceAssert(response != NULL);
   SDeviceAssert(handle->IsInitialized == true);

   if(request->BytesCount < __MODBUS_RTU_EMPTY_ADU_SIZE || request->BytesCount > __MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REQUEST_SIZE_ERROR);
      return false;
   }

   const __MODBUS_RTU_ADU(request->BytesCount - __MODBUS_RTU_EMPTY_ADU_SIZE) *requestAdu = request->Bytes;
   ModbusRtuSDeviceRequestType requestType;

   if(requestAdu->SlaveAddress != handle->Settings.SlaveAddress)
   {
      if(requestAdu->SlaveAddress != __MODBUS_RTU_SDEVICE_BROADCAST_REQUEST_SLAVE_ADDRESS)
         return false;

      requestType = MODBUS_RTU_SDEVICE_REQUEST_TYPE_BROADCAST;
   }
   else
   {
      requestType = MODBUS_RTU_SDEVICE_REQUEST_TYPE_NORMAL;
   }

   if(requestAdu->Crc16 != ModbusRtuCrcCompute(request->Bytes, request->BytesCount - sizeof(ModbusRtuCrcType)))
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REQUEST_CRC_ERROR);
      return false;
   }

   ModbusSDeviceRequest requestPdu = { requestAdu->PduBytes, sizeof(requestAdu->PduBytes) };
   ModbusSDeviceResponse responsePdu = { ((__MODBUS_RTU_ADU(0) *)response->Bytes)->PduBytes };
   ModbusProcessingParameters processingParameters =
   {
      .RequestContext = &(ModbusRtuSDeviceRequestData)
      {
         .Common.ModbusType = MODBUS_SDEVICE_MODBUS_TYPE_RTU,
         .RequestType = requestType
      }
   };

   if(ModbusProcessPdu((SDeviceCommonHandle *)handle, processingParameters, &requestPdu, &responsePdu) != true)
      return false;

   __MODBUS_RTU_ADU(responsePdu.BytesCount) *responseAdu = response->Bytes;

   if(requestType == MODBUS_RTU_SDEVICE_REQUEST_TYPE_BROADCAST)
   {
      response->BytesCount = 0;
      return true;
   }

   responseAdu->SlaveAddress = handle->Settings.SlaveAddress;
   responseAdu->Crc16 = ModbusRtuCrcCompute(response->Bytes, sizeof(*responseAdu) - sizeof(ModbusRtuCrcType));

   response->BytesCount = sizeof(*responseAdu);
   return true;
}
