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
} TcpAduData;

bool ModbusTcpTryProcessMbapHeader(__SDEVICE_HANDLE(ModbusTcp) *handle, ModbusRequest *request, size_t *lengthToReceive)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(request != NULL);
   SDeviceAssert(lengthToReceive != NULL);
   SDeviceAssert(handle->IsInitialized == true);

   if(request->Size != __MODBUS_TCP_MBAP_HEADER_SIZE)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_RUNTIME_ERROR_WRONG_REQUEST_SIZE);
      return false;
   }

   const TcpAduData *requestAdu = (const TcpAduData *)request->Bytes;

   if(requestAdu->ProtocolId != __MODBUS_TCP_PROTOCOL_ID)
      return false;

   memcpy(handle->Dynamic.MbapHeaderBuffer, request->Bytes, __MODBUS_TCP_MBAP_HEADER_SIZE);

   *lengthToReceive = requestAdu->PacketSize;
   return true;
}

bool ModbusTcpTryProcessRequest(__SDEVICE_HANDLE(ModbusTcp) *handle, ModbusRequest *request, ModbusResponse *response)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(request != NULL);
   SDeviceAssert(response != NULL);
   SDeviceAssert(handle->IsInitialized == true);

   TcpAduData *mbapHeader = (TcpAduData *)handle->Dynamic.MbapHeaderBuffer;
   if(request->Size > __MODBUS_TCP_MAX_MESSAGE_SIZE - __MODBUS_TCP_MBAP_HEADER_SIZE ||
      request->Size + __SIZEOF_MEMBER(TcpAduData, SlaveAddress) != mbapHeader->PacketSize)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_RUNTIME_ERROR_WRONG_REQUEST_SIZE);
      return false;
   }

   TcpAduData *responseAdu = (TcpAduData *)response->Bytes;
   ModbusResponse responsePdu = { responseAdu->PduBytes };
   ModbusProcessingParameters processingParameters =
   {
      .RequestContext = &(ModbusTcpRequestData)
      {
         .Common.ModbusType = MODBUS_MODBUS_TYPE_TCP,
         .SlaveAddress = mbapHeader->SlaveAddress
      }
   };

   if(TryProcessModbusPdu((SDeviceCommonHandle *)handle, processingParameters, request, &responsePdu) != true)
      return false;

   responseAdu->ProtocolId = __MODBUS_TCP_PROTOCOL_ID;
   responseAdu->TransactionId = mbapHeader->TransactionId;
   responseAdu->SlaveAddress = mbapHeader->SlaveAddress;
   responseAdu->PacketSize = responsePdu.Size + sizeof(TcpAduData) - offsetof(TcpAduData, SlaveAddress);

   response->Size = responsePdu.Size + sizeof(TcpAduData);
   return true;
}
