#include "private_tcp.h"
#include "PDU/pdu.h"

#include "SDeviceCore/heap.h"
#include "SDeviceCore/errors.h"
#include "SDeviceCore/common.h"

#include <memory.h>

#define MODBUS_TCP_MBAP_HEADER_PROTOCOL_ID 0x0000

typedef struct __attribute__((packed, scalar_storage_order("big-endian")))
{
   uint16_t TransactionId;
   uint16_t ProtocolId;
   uint16_t PacketSize;
   uint8_t SlaveAddress;
   uint8_t PduBytes[];
} ModbusTcpAduData;

SDEVICE_STRING_NAME_DEFINITION(ModbusTcp);

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusTcp, init, owner, identifier, context)
{
   SDeviceAssert(init != NULL);

   const ThisInitData *_init = init;

   SDeviceAssert(_init->RegistersCallbacks.Read != NULL);
   SDeviceAssert(_init->RegistersCallbacks.Write != NULL);

   ThisHandle *handle = SDeviceMalloc(sizeof(SDEVICE_HANDLE(ModbusTcp)));
   handle->Header = (SDeviceHandleHeader)
   {
      .Context = context,
      .OwnerHandle = owner,
      .SDeviceStringName = SDEVICE_STRING_NAME(ModbusTcp),
      .LatestStatus = MODBUS_TCP_SDEVICE_STATUS_OK,
      .Identifier = identifier
   };
   handle->Init = *_init;

   return handle;
}

SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusTcp, handlePointer)
{
   SDeviceAssert(handlePointer != NULL);

   ThisHandle **_handlePointer = handlePointer;
   ThisHandle *handle = *_handlePointer;

   SDeviceAssert(handle != NULL);

   SDeviceFree(handle);
   *_handlePointer = NULL;
}

bool ModbusTcpSDeviceTryProcessMbapHeader(ThisHandle *handle,
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

   if(mbapHeader->ProtocolId != MODBUS_TCP_MBAP_HEADER_PROTOCOL_ID)
      return false;

   handle->Runtime.MbapHeaderData.PacketSize = mbapHeader->PacketSize;
   handle->Runtime.MbapHeaderData.SlaveAddress = mbapHeader->SlaveAddress;
   handle->Runtime.MbapHeaderData.TransactionId = mbapHeader->TransactionId;

   *lengthToReceive = handle->Runtime.MbapHeaderData.PacketSize;
   return true;
}

bool ModbusTcpSDeviceTryProcessRequest(ThisHandle *handle,
                                       const ModbusSDeviceRequest *request,
                                       ModbusSDeviceResponse *response)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(request != NULL);
   SDeviceAssert(response != NULL);
   SDeviceAssert(request->Data != NULL);
   SDeviceAssert(response->Data != NULL);

   if(request->Size > MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE - MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE ||
      request->Size + SIZEOF_MEMBER(ModbusTcpAduData, SlaveAddress) != handle->Runtime.MbapHeaderData.PacketSize)
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
         .SlaveAddress = handle->Runtime.MbapHeaderData.SlaveAddress
      }
   };

   if(TryProcessRequestPdu(handle, &processingParameters, request, &responsePdu) != true)
      return false;

   responseAdu->ProtocolId = MODBUS_TCP_MBAP_HEADER_PROTOCOL_ID;
   responseAdu->TransactionId = handle->Runtime.MbapHeaderData.TransactionId;
   responseAdu->SlaveAddress = handle->Runtime.MbapHeaderData.SlaveAddress;
   responseAdu->PacketSize = responsePdu.Size + SIZEOF_MEMBER(ModbusTcpAduData, SlaveAddress);

   response->Size = responsePdu.Size + sizeof(ModbusTcpAduData);

   return true;
}
