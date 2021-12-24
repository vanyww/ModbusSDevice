#include "../Inc/ModbusSDevice/core_tcp.h"
#include "PDU/pdu.h"

#include <memory.h>

#define __MODBUS_TCP_PROTOCOL_ID 0x0000

typedef struct __attribute__((scalar_storage_order("big-endian"), packed))
{
   uint16_t TransactionId;
   uint16_t ProtocolId;
   uint16_t PacketSize;
   uint8_t SlaveAddress;
   uint8_t PduBytes[];
} ModbusTcpAduData;

bool ModbusTcpSDeviceTryProcessMbapHeader(__SDEVICE_HANDLE(ModbusTcp) *handle,
                                          ModbusSDeviceRequest *request,
                                          size_t *lengthToReceive)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(request != NULL);
   SDeviceAssert(lengthToReceive != NULL);
   SDeviceAssert(handle->IsInitialized == true);

   if(request->BytesCount != __MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REQUEST_SIZE_ERROR);
      return false;
   }

   const ModbusTcpAduData *requestAdu = (const ModbusTcpAduData *)request->Bytes;

   if(requestAdu->ProtocolId != __MODBUS_TCP_PROTOCOL_ID)
      return false;

   memcpy(handle->Dynamic.MbapHeaderBuffer, request->Bytes, __MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE);

   *lengthToReceive = requestAdu->PacketSize;

   return true;
}

bool ModbusTcpSDeviceTryProcessRequest(__SDEVICE_HANDLE(ModbusTcp) *handle,
                                       ModbusSDeviceRequest *request,
                                       ModbusSDeviceResponse *response)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(request != NULL);
   SDeviceAssert(response != NULL);
   SDeviceAssert(handle->IsInitialized == true);

   ModbusTcpAduData *mbapHeader = (ModbusTcpAduData *)handle->Dynamic.MbapHeaderBuffer;

   if(request->BytesCount > __MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE - __MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE ||
      request->BytesCount + sizeof(((ModbusTcpAduData *)NULL)->SlaveAddress) != mbapHeader->PacketSize)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REQUEST_SIZE_ERROR);
      return false;
   }

   ModbusTcpAduData *responseAdu = (ModbusTcpAduData *)response->Bytes;

   ModbusSDeviceResponse responsePdu = { responseAdu->PduBytes };
   ModbusProcessingParameters processingParameters =
   {
      .RequestContext = &(ModbusTcpSDeviceRequestData)
      {
         .Common.ModbusType = MODBUS_SDEVICE_MODBUS_TYPE_TCP,
         .SlaveAddress = mbapHeader->SlaveAddress
      }
   };

   if(ModbusProcessPdu((SDeviceCommonHandle *)handle, processingParameters, request, &responsePdu) != true)
      return false;

   responseAdu->ProtocolId = __MODBUS_TCP_PROTOCOL_ID;
   responseAdu->TransactionId = mbapHeader->TransactionId;
   responseAdu->SlaveAddress = mbapHeader->SlaveAddress;
   responseAdu->PacketSize = responsePdu.BytesCount +
                             sizeof(ModbusTcpAduData) -
                             offsetof(ModbusTcpAduData, SlaveAddress);

   response->BytesCount = responsePdu.BytesCount + sizeof(ModbusTcpAduData);
   return true;
}
