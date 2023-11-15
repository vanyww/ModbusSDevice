#include "private.h"

#include "SDeviceCore/heap.h"
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

SDEVICE_IDENTITY_BLOCK_DEFINITION(ModbusTcp,
                                  NULL,
                                  ((const SDeviceUuid)
                                  {
                                     .High = MODBUS_TCP_SDEVICE_UUID_HIGH,
                                     .Low  = MODBUS_TCP_SDEVICE_UUID_LOW
                                  }),
                                  ((const SDeviceVersion)
                                  {
                                     .Major = MODBUS_TCP_SDEVICE_VERSION_MAJOR,
                                     .Minor = MODBUS_TCP_SDEVICE_VERSION_MINOR,
                                     .Patch = MODBUS_TCP_SDEVICE_VERSION_PATCH
                                  }));

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusTcp, init, owner, identifier, context)
{
   SDeviceAssert(init != NULL);

   const ThisInitData *_init = init;

   SDeviceAssert(_init->BaseInit.ReadOperation != NULL);
   SDeviceAssert(_init->BaseInit.WriteOperation != NULL);

   ThisHandle *handle = SDeviceAllocHandle(sizeof(ThisInitData), sizeof(ThisRuntimeData));
   handle->Header = (SDeviceHandleHeader)
   {
      .Context       = context,
      .OwnerHandle   = owner,
      .IdentityBlock = &SDEVICE_IDENTITY_BLOCK(ModbusTcp),
      .LatestStatus  = MODBUS_TCP_SDEVICE_STATUS_OK,
      .Identifier    = identifier
   };
   *handle->Init = *_init;

   return handle;
}

SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusTcp, handlePointer)
{
   SDeviceAssert(handlePointer != NULL);

   ThisHandle **_handlePointer = handlePointer;
   ThisHandle *handle = *_handlePointer;

   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   SDeviceFreeHandle(handle);
   *_handlePointer = NULL;
}

bool ModbusTcpSDeviceTryProcessMbapHeader(ThisHandle *handle,
                                          const void *mbapHeaderData,
                                          size_t     *requestSizeToReceive)
{
   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   SDeviceAssert(mbapHeaderData != NULL);
   SDeviceAssert(requestSizeToReceive != NULL);

   const TcpMbapHeader *mbapHeader = mbapHeaderData;

   if(mbapHeader->ProtocolId != __builtin_bswap16(MODBUS_TCP_MBAP_HEADER_PROTOCOL_ID))
   {
      SDeviceLogStatus(handle, MODBUS_TCP_SDEVICE_STATUS_WRONG_PROTOCOL_ID);
      return false;
   }

   handle->Runtime->MbapHeaderData.PacketSize = __builtin_bswap16(mbapHeader->PacketSize);
   handle->Runtime->MbapHeaderData.SlaveAddress = mbapHeader->SlaveAddress;
   handle->Runtime->MbapHeaderData.TransactionId = mbapHeader->TransactionId;

   *requestSizeToReceive = handle->Runtime->MbapHeaderData.PacketSize - 1;

   return true;
}

bool ModbusTcpSDeviceTryProcessRequest(ThisHandle            *handle,
                                       ModbusTcpSDeviceInput  input,
                                       ModbusTcpSDeviceOutput output)
{
   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   SDeviceAssert(input.RequestData != NULL);
   SDeviceAssert(output.ResponseData != NULL);
   SDeviceAssert(output.ResponseSize != NULL);

   size_t expectedPacketSize;
   if(!TRY_ADD_INT_CHECKED(input.RequestSize, SIZEOF_MEMBER(TcpMbapHeader, SlaveAddress), &expectedPacketSize) ||
      expectedPacketSize != handle->Runtime->MbapHeaderData.PacketSize)
   {
      SDeviceLogStatus(handle, MODBUS_TCP_SDEVICE_STATUS_WRONG_REQUEST_SIZE);
      return false;
   }

   TcpAdu *response = output.ResponseData;
   ModbusTcpSDeviceOperationContext context = { .SlaveAddress = handle->Runtime->MbapHeaderData.SlaveAddress };

   size_t pduResponseSize;
   bool wasPduProcessingSuccessful =
         ModbusSDeviceTryProcessRequestPdu((SDEVICE_HANDLE(Modbus) *)handle,
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
                                           });

   if(wasPduProcessingSuccessful)
   {
      response->MbapHeader = (TcpMbapHeader)
      {
         .ProtocolId    = __builtin_bswap16(MODBUS_TCP_MBAP_HEADER_PROTOCOL_ID),
         .TransactionId = handle->Runtime->MbapHeaderData.TransactionId,
         .SlaveAddress  = handle->Runtime->MbapHeaderData.SlaveAddress,
         .PacketSize    = __builtin_bswap16(pduResponseSize + SIZEOF_MEMBER(TcpAdu, MbapHeader.SlaveAddress))
      };

      *output.ResponseSize = EMPTY_TCP_ADU_SIZE + pduResponseSize;
   }

   return wasPduProcessingSuccessful;
}
