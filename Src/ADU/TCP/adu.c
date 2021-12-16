#include "adu.h"
#include "../../PDU/pdu.h"
#include "../../../Inc/ModbusSDevice/tcp_defs.h"

#define __MODBUS_TCP_PROTOCOL_ID ((uint16_t)0x0000)
#define __MODBUS_TCP_ADU_DATA(buffer) ((ModbusTcpAduData *)buffer)

typedef struct  __attribute__((scalar_storage_order("big-endian"), packed))
{
   uint16_t TransactionId;
   uint16_t ProtocolId;
   uint16_t PacketSize;
   uint8_t SlaveAddress;
   uint8_t PduBytes[];
} ModbusTcpAduData;

bool ModbusTcpTryProcessAdu(__SDEVICE_HANDLE(Modbus) *handle,
                            ModbusSDeviceRequest *request,
                            ModbusSDeviceResponse *response)
{
   if(request->BytesCount < sizeof(ModbusTcpAduData) ||
      request->BytesCount > __MODBUS_SDEVICE_TCP_MAX_MESSAGE_SIZE)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REQUEST_SIZE_ERROR);
      return false;
   }

   const ModbusTcpAduData *requestAdu = request->Bytes;
   ModbusTcpAduData *responseAdu = response->Bytes;

   if(requestAdu->ProtocolId != __MODBUS_TCP_PROTOCOL_ID)
      return false;

   if(requestAdu->PacketSize != request->BytesCount - offsetof(ModbusTcpAduData, SlaveAddress))
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REQUEST_SIZE_ERROR);
      return false;
   }

   ModbusSDeviceRequest requestPdu =
   {
      .Bytes = &requestAdu->PduBytes,
      .BytesCount = request->BytesCount - offsetof(ModbusTcpAduData, PduBytes)
   };
   ModbusSDeviceResponse responsePdu =
   {
      .Bytes = &responseAdu->PduBytes
   };
   ModbusProcessingParameters processingParameters =
   {
      .RequestContext = &(const ModbusSDeviceTcpRequestData)
      {
         .SlaveAddress = requestAdu->SlaveAddress
      }
   };

   if(ModbusProcessPdu(handle, processingParameters, &requestPdu, &responsePdu) != true)
      return false;

   responseAdu->ProtocolId = __MODBUS_TCP_PROTOCOL_ID;
   responseAdu->TransactionId = requestAdu->TransactionId;
   responseAdu->SlaveAddress = requestAdu->SlaveAddress;
   responseAdu->PacketSize = responsePdu.BytesCount +
                             sizeof(ModbusTcpAduData) -
                             offsetof(ModbusTcpAduData, SlaveAddress);

   response->BytesCount = responsePdu.BytesCount + sizeof(ModbusTcpAduData);
   return true;
}
