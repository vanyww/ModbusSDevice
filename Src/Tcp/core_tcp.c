#include "private_tcp.h"
#include "../Base/internal_base.h"

#include "SDeviceCore/heap.h"
#include "SDeviceCore/errors.h"
#include "SDeviceCore/common.h"

#define MODBUS_TCP_MBAP_HEADER_PROTOCOL_ID 0x0000
#define EMPTY_TCP_ADU_SIZE (sizeof(TcpAdu) - SIZEOF_MEMBER(TcpAdu, Pdu))

typedef struct __attribute__((packed, __may_alias__))
{
   uint16_t TransactionId;
   uint16_t ProtocolId;
   uint16_t PacketSize;
   uint8_t  SlaveAddress;
} TcpMbapHeader;

_Static_assert(sizeof(TcpMbapHeader) == MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE, "Modbus-TCP MBAP header size wrong!");

typedef struct __attribute__((packed, __may_alias__))
{
   TcpMbapHeader MbapHeader;
   MessagePdu    Pdu;
} TcpAdu;

SDEVICE_STRING_NAME_DEFINITION(ModbusTcp);

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusTcp, init, owner, identifier, context)
{
   SDeviceAssert(init != NULL);

   const ThisInitData *_init = init;

   SDeviceAssert(_init->BaseInit.ReadOperation != NULL);
   SDeviceAssert(_init->BaseInit.WriteOperation != NULL);

   ThisHandle *handle = SDeviceMalloc(sizeof(ThisHandle));
   handle->Header = (SDeviceHandleHeader)
   {
      .Context           = context,
      .OwnerHandle       = owner,
      .SDeviceStringName = SDEVICE_STRING_NAME(ModbusTcp),
      .LatestStatus      = MODBUS_TCP_SDEVICE_STATUS_OK,
      .Identifier        = identifier
   };
   handle->Init = *_init;
   handle->Runtime = (ThisRuntimeData)
   {
      .BaseHandle = SDEVICE_CREATE_HANDLE(Modbus)(&_init->BaseInit, handle, 0, NULL),
   };

   return handle;
}

SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusTcp, handlePointer)
{
   SDeviceAssert(handlePointer != NULL);

   ThisHandle **_handlePointer = handlePointer;
   ThisHandle *handle = *_handlePointer;

   SDeviceAssert(handle != NULL);

   SDEVICE_DISPOSE_HANDLE(Modbus)(&handle->Runtime.BaseHandle);

   SDeviceFree(handle);
   *_handlePointer = NULL;
}

bool ModbusTcpSDeviceTryProcessMbapHeader(ThisHandle *handle,
                                          const void *mbapHeaderData,
                                          size_t     *requestSizeToReceive)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(mbapHeaderData != NULL);
   SDeviceAssert(requestSizeToReceive != NULL);

   const TcpMbapHeader *mbapHeader = mbapHeaderData;

   if(mbapHeader->ProtocolId != __builtin_bswap16(MODBUS_TCP_MBAP_HEADER_PROTOCOL_ID))
      return false;

   handle->Runtime.MbapHeaderData.PacketSize = __builtin_bswap16(mbapHeader->PacketSize);
   handle->Runtime.MbapHeaderData.SlaveAddress = mbapHeader->SlaveAddress;
   handle->Runtime.MbapHeaderData.TransactionId = mbapHeader->TransactionId;

   *requestSizeToReceive = handle->Runtime.MbapHeaderData.PacketSize;

   return true;
}

bool ModbusTcpSDeviceTryProcessRequest(ThisHandle            *handle,
                                       ModbusTcpSDeviceInput  input,
                                       ModbusTcpSDeviceOutput output)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(input.RequestData != NULL);
   SDeviceAssert(output.ResponseData != NULL);
   SDeviceAssert(output.ResponseSize != NULL);

   size_t expectedPacketSize;
   if(__builtin_add_overflow(input.RequestSize, SIZEOF_MEMBER(TcpMbapHeader, SlaveAddress), &expectedPacketSize) ||
      expectedPacketSize != handle->Runtime.MbapHeaderData.PacketSize)
   {
      SDeviceLogStatus(handle, MODBUS_TCP_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return false;
   }

   TcpAdu *response = output.ResponseData;
   ModbusTcpSDeviceOperationContext context = { .SlaveAddress = handle->Runtime.MbapHeaderData.SlaveAddress };

   size_t pduResponseSize;
   if(!ModbusSDeviceTryProcessRequestPdu(handle->Runtime.BaseHandle,
                                         &context,
                                         (PduInput)
                                         {
                                            .Pdu     = (const MessagePdu *)input.RequestData,
                                            .PduSize = input.RequestSize
                                         },
                                         (PduOutput)
                                         {
                                            .Pdu     = &response->Pdu,
                                            .PduSize = &pduResponseSize
                                         }))
   {
      return false;
   }

   response->MbapHeader = (TcpMbapHeader)
   {
      .ProtocolId    = __builtin_bswap16(MODBUS_TCP_MBAP_HEADER_PROTOCOL_ID),
      .TransactionId = handle->Runtime.MbapHeaderData.TransactionId,
      .SlaveAddress  = handle->Runtime.MbapHeaderData.SlaveAddress,
      .PacketSize    = __builtin_bswap16(pduResponseSize + SIZEOF_MEMBER(TcpAdu, MbapHeader.SlaveAddress))
   };

   *output.ResponseSize = EMPTY_TCP_ADU_SIZE + pduResponseSize;

   return true;
}
