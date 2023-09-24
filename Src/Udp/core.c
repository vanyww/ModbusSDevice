#include "private.h"
#include "BtuBlock/btu_block.h"

#include "SDeviceCore/heap.h"

#define MODBUS_UDP_MBAP_HEADER_PROTOCOL_ID 0x0000
#define MODBUS_UDP_BTU_MBAP_HEADER_PROTOCOL_ID 0x0074

#define EMPTY_UDP_ADU_SIZE (sizeof(UdpAdu) - SIZEOF_MEMBER(UdpAdu, Data))

typedef struct __attribute__((packed, __may_alias__))
{
   uint16_t TransactionId;
   uint16_t ProtocolId;
   uint16_t PacketSize;
   uint8_t  SlaveAddress;
} UdpMbapHeader;

typedef struct __attribute__((packed, __may_alias__))
{
   UdpMbapHeader MbapHeader;
   union
   {
      MessagePdu  AsPdu;
      UdpBtuBlock AsBtuBlock;
   } Data;
} UdpAdu;

SDEVICE_IDENTITY_BLOCK_DEFINITION(ModbusUdp,
                                  ((const SDeviceUuid)
                                  {
                                     .High = MODBUS_UDP_SDEVICE_UUID_HIGH,
                                     .Low  = MODBUS_UDP_SDEVICE_UUID_LOW
                                  }),
                                  ((const SDeviceVersion)
                                  {
                                     .Major = MODBUS_UDP_SDEVICE_VERSION_MAJOR,
                                     .Minor = MODBUS_UDP_SDEVICE_VERSION_MINOR,
                                     .Patch = MODBUS_UDP_SDEVICE_VERSION_PATCH
                                  }));

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusUdp, init, owner, identifier, context)
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
      .IdentityBlock = &SDEVICE_IDENTITY_BLOCK(ModbusUdp),
      .LatestStatus  = MODBUS_UDP_SDEVICE_STATUS_OK,
      .Identifier    = identifier
   };
   *handle->Init = *_init;

   return handle;
}

SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusUdp, handlePointer)
{
   SDeviceAssert(handlePointer != NULL);

   ThisHandle **_handlePointer = handlePointer;
   ThisHandle *handle = *_handlePointer;

   SDeviceFreeHandle(handle);
   *_handlePointer = NULL;
}

bool ModbusUdpSDeviceTryProcessRequest(ThisHandle            *handle,
                                       ModbusUdpSDeviceInput  input,
                                       ModbusUdpSDeviceOutput output)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(input.RequestData != NULL);
   SDeviceAssert(output.ResponseData != NULL);
   SDeviceAssert(output.ResponseSize != NULL);

   const UdpAdu *request = input.RequestData;
   UdpAdu *response = output.ResponseData;

   if(input.RequestSize < EMPTY_UDP_ADU_SIZE ||
      __builtin_bswap16(request->MbapHeader.PacketSize) !=
            input.RequestSize - (EMPTY_UDP_ADU_SIZE - SIZEOF_MEMBER(UdpAdu, MbapHeader.SlaveAddress)))
   {
      SDeviceLogStatus(handle, MODBUS_UDP_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return false;
   }

   size_t responseDataSize;
   bool wasInnerProcessingSuccessful;
   uint16_t protocolId = request->MbapHeader.ProtocolId;
   ModbusUdpSDeviceOperationContext context = { .SlaveAddress = request->MbapHeader.SlaveAddress };

   switch(protocolId)
   {
      case __builtin_bswap16(MODBUS_UDP_MBAP_HEADER_PROTOCOL_ID):
         context.IsBroadcast = input.IsBroadcast;
         wasInnerProcessingSuccessful =
               ModbusSDeviceTryProcessRequestPdu((SDEVICE_HANDLE(Modbus) *)handle,
                                                 &context,
                                                 (PduInput)
                                                 {
                                                    .Pdu     = &request->Data.AsPdu,
                                                    .PduSize = input.RequestSize - EMPTY_UDP_ADU_SIZE
                                                 },
                                                 (PduOutput)
                                                 {
                                                    .Pdu     = &response->Data.AsPdu,
                                                    .PduSize = &responseDataSize
                                                 });
         break;

      case __builtin_bswap16(MODBUS_UDP_BTU_MBAP_HEADER_PROTOCOL_ID):
         context.IsBroadcast = false;
         wasInnerProcessingSuccessful =
               TryProcessRequestUdpBtuBlock(handle,
                                            &context,
                                            (UdpBtuBlockInput)
                                            {
                                               .BtuBlock     = &request->Data.AsBtuBlock,
                                               .BtuBlockSize = input.RequestSize - EMPTY_UDP_ADU_SIZE
                                            },
                                            (UdpBtuBlockOutput)
                                            {
                                               .BtuBlock     = &response->Data.AsBtuBlock,
                                               .BtuBlockSize = &responseDataSize
                                            });
         break;

      default:
         SDeviceLogStatus(handle, MODBUS_UDP_SDEVICE_STATUS_WRONG_PROTOCOL_ID);
         return false;
   }

   if(wasInnerProcessingSuccessful)
   {
      response->MbapHeader = (UdpMbapHeader)
      {
         .ProtocolId    = protocolId,
         .TransactionId = request->MbapHeader.TransactionId,
         .SlaveAddress  = context.SlaveAddress,
         .PacketSize    = __builtin_bswap16(responseDataSize + SIZEOF_MEMBER(UdpAdu, MbapHeader.SlaveAddress))
      };

      *output.ResponseSize = EMPTY_UDP_ADU_SIZE + responseDataSize;
   }

   return wasInnerProcessingSuccessful;
}
