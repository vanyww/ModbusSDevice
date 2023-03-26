#include "PDU/pdu.h"

#include "SDeviceCore/heap.h"
#include "SDeviceCore/errors.h"
#include "SDeviceCore/common.h"

#include <memory.h>

#define MODBUS_TCP_PROTOCOL_ID 0x0000

typedef struct __attribute__((packed, scalar_storage_order("big-endian")))
{
   uint16_t TransactionId;
   uint16_t ProtocolId;
   uint16_t PacketSize;
   uint8_t SlaveAddress;
   uint8_t PduBytes[];
} ModbusTcpAduData;

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusTcp, init, parent, identifier, context)
{
   SDeviceAssert(init != NULL);

   const SDEVICE_INIT_DATA(ModbusTcp) *_init = init;

   SDeviceAssert(_init->RegistersCallbacks.ReadRegisters != NULL);
   SDeviceAssert(_init->RegistersCallbacks.WriteRegisters != NULL);

   SDEVICE_HANDLE(ModbusTcp) *handle = SDeviceMalloc(sizeof(SDEVICE_HANDLE(ModbusTcp)));

   SDeviceAssert(handle != NULL);

   handle->Init = *_init;
   handle->Header = (SDeviceHandleHeader)
   {
      .Context = context,
      .ParentHandle = parent,
      .Identifier = identifier,
      .LatestStatus = MODBUS_TCP_SDEVICE_STATUS_OK
   };

   return handle;
}

SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusTcp, handlePointer)
{
   SDeviceAssert(handlePointer != NULL);

   SDEVICE_HANDLE(ModbusTcp) **_handlePointer = handlePointer;
   SDEVICE_HANDLE(ModbusTcp) *handle = *_handlePointer;

   SDeviceAssert(handle != NULL);

   SDeviceFree(handle);
   *_handlePointer = NULL;
}

bool ModbusTcpSDeviceTryProcessMbapHeader(SDEVICE_HANDLE(ModbusTcp) *handle,
                                          const ModbusSDeviceRequest *request,
                                          size_t *lengthToReceive)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(request != NULL);
   SDeviceAssert(request->Data != NULL);
   SDeviceAssert(lengthToReceive != NULL);

   if(request->Size != MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE)
   {
      SDeviceLogStatus(handle, MODBUS_TCP_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return false;
   }

   const ModbusTcpAduData *mbapHeader = (const ModbusTcpAduData *)request->Data;

   if(mbapHeader->ProtocolId != MODBUS_TCP_PROTOCOL_ID)
      return false;

   *lengthToReceive = mbapHeader->PacketSize;
   memcpy(handle->Runtime.MbapHeaderBuffer, mbapHeader, MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE);

   return true;
}

bool ModbusTcpSDeviceTryProcessRequest(SDEVICE_HANDLE(ModbusTcp) *handle,
                                       const ModbusSDeviceRequest *request,
                                       ModbusSDeviceResponse *response)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(request != NULL);
   SDeviceAssert(response != NULL);
   SDeviceAssert(request->Data != NULL);
   SDeviceAssert(response->Data != NULL);

   const ModbusTcpAduData *mbapHeader = (const ModbusTcpAduData *)handle->Runtime.MbapHeaderBuffer;

   if(request->Size > MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE - MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE ||
      request->Size + SIZEOF_MEMBER(ModbusTcpAduData, SlaveAddress) != mbapHeader->PacketSize)
   {
      SDeviceLogStatus(handle, MODBUS_TCP_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return false;
   }

   ModbusTcpAduData *responseAdu = (ModbusTcpAduData *)response->Data;
   ModbusSDeviceResponse responsePdu = { responseAdu->PduBytes };

   const ModbusProcessingParameters processingParameters =
   {
      .RegistersCallbacks = &handle->Init.RegistersCallbacks,
      .RequestContext = &(const ModbusTcpSDeviceRequestContext)
      {
         .Common.ProtocolType = MODBUS_SDEVICE_PROTOCOL_TYPE_TCP,
         .SlaveAddress = mbapHeader->SlaveAddress
      }
   };

   if(TryProcessRequestPdu(handle, &processingParameters, request, &responsePdu) != true)
      return false;

   responseAdu->ProtocolId = MODBUS_TCP_PROTOCOL_ID;
   responseAdu->TransactionId = mbapHeader->TransactionId;
   responseAdu->SlaveAddress = mbapHeader->SlaveAddress;
   responseAdu->PacketSize = responsePdu.Size + SIZEOF_MEMBER(ModbusTcpAduData, SlaveAddress);

   response->Size = responsePdu.Size + sizeof(ModbusTcpAduData);

   return true;
}
