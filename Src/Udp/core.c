#include "BtuBlock/btu_block.h"

#include "SDeviceCore/heap.h"

#define MODBUS_UDP_MBAP_HEADER_PROTOCOL_ID 0x0000
#define MODBUS_UDP_BTU_MBAP_HEADER_PROTOCOL_ID 0x0074
#define EMPTY_UDP_ADU_SIZE sizeof(UdpAdu)

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

typedef struct __attribute__((packed, may_alias))
{
   uint16_t TransactionIdx;
   uint16_t ProtocolIdx;
   uint16_t PacketSize;
   uint8_t  SlaveAddress;
} UdpMbapHeader;

typedef struct __attribute__((packed, may_alias))
{
   UdpMbapHeader MbapHeader;
   uint8_t       PduBytes[];
} UdpAdu;

typedef bool (* RequestPduTryProcessFunction)(void                 *handle,
                                              ProcessingStageInput  input,
                                              ProcessingStageOutput output);

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusUdp, init, owner, identifier, context)
{
   SDeviceAssert(init != NULL);

   const ThisInitData *_init = init;

   SDeviceAssert(_init->Base.ReadOperation != NULL);
   SDeviceAssert(_init->Base.WriteOperation != NULL);

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

   handle->Runtime->Base.SupportsBroadcasts = true;

   return handle;
}

SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusUdp, handlePointer)
{
   SDeviceAssert(handlePointer != NULL);

   ThisHandle **_handlePointer = handlePointer;
   ThisHandle *handle = *_handlePointer;

   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   SDeviceFreeHandle(handle);
   *_handlePointer = NULL;
}

SDEVICE_GET_PROPERTY_DECLARATION(ModbusUdp, BtuAddress, handle, value)
{
   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   SDeviceAssert(value != NULL);

   ThisHandle *_handle = handle;
   memcpy(value, &_handle->Runtime->BtuAddress, sizeof(_handle->Runtime->BtuAddress));

   return SDEVICE_PROPERTY_STATUS_OK;
}

SDEVICE_SET_PROPERTY_DECLARATION(ModbusUdp, BtuAddress, handle, value)
{
   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   SDeviceAssert(value != NULL);

   ThisHandle *_handle = handle;
   memcpy(_handle->Runtime->BtuAddress, value, sizeof(SDEVICE_PROPERTY_TYPE(ModbusUdp, BtuAddress)));

   return SDEVICE_PROPERTY_STATUS_OK;
}

bool ModbusUdpSDeviceTryProcessRequest(ThisHandle *handle,
                                       ThisInput   input,
                                       ThisOutput  output)
{
   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   SDeviceAssert(input.RequestData != NULL);
   SDeviceAssert(output.ResponseData != NULL);
   SDeviceAssert(output.ResponseSize != NULL);

   const UdpAdu *request = input.RequestData;
   UdpAdu *response = output.ResponseData;

   if(input.RequestSize < EMPTY_UDP_ADU_SIZE ||
      input.RequestSize - (EMPTY_UDP_ADU_SIZE - SIZEOF_MEMBER(UdpAdu, MbapHeader.SlaveAddress)) !=
            SWAP_UINT16_BYTES(request->MbapHeader.PacketSize))
   {
      SDeviceLogStatus(handle, MODBUS_UDP_SDEVICE_STATUS_WRONG_REQUEST_SIZE);
      return false;
   }

   uint16_t protocolIdx = request->MbapHeader.ProtocolIdx;
   uint16_t transactionIdx = request->MbapHeader.TransactionIdx;
   uint8_t slaveAddress = request->MbapHeader.SlaveAddress;
   bool isBroadcastRequest;

   RequestPduTryProcessFunction pduProcessor;
   switch(protocolIdx)
   {
      case SWAP_UINT16_BYTES(MODBUS_UDP_MBAP_HEADER_PROTOCOL_ID):
         isBroadcastRequest = input.IsBroadcast;
         pduProcessor = ModbusSDeviceBaseTryProcessRequestPdu;
         break;

      case SWAP_UINT16_BYTES(MODBUS_UDP_BTU_MBAP_HEADER_PROTOCOL_ID):
         isBroadcastRequest = false;
         pduProcessor = TryProcessRequestUdpBtuBlock;
         break;

      default:
         if(!input.IsBroadcast)
            SDeviceLogStatus(handle, MODBUS_UDP_SDEVICE_STATUS_WRONG_PROTOCOL_ID);

         return false;
   }

   size_t responseDataSize;
   bool wasProcessingSuccessful =
         pduProcessor(handle,
                      (ProcessingStageInput)
                      {
                         .RequestData       = request->PduBytes,
                         .OperationContext  = &(const ThisOperationContext)
                         {
                            .Base.IsBroadcast = isBroadcastRequest,
                            .SlaveAddress = slaveAddress
                         },
                         .RequestSize       = input.RequestSize - EMPTY_UDP_ADU_SIZE,
                         .IsOutputMandatory = true
                      },
                      (ProcessingStageOutput)
                      {
                         .ResponseData = response->PduBytes,
                         .ResponseSize = &responseDataSize
                      });

   if(wasProcessingSuccessful)
   {
      response->MbapHeader = (UdpMbapHeader)
      {
         .ProtocolIdx    = protocolIdx,
         .TransactionIdx = transactionIdx,
         .SlaveAddress   = slaveAddress,
         .PacketSize     = SWAP_UINT16_BYTES(responseDataSize + SIZEOF_MEMBER(UdpAdu, MbapHeader.SlaveAddress))
      };

      *output.ResponseSize = EMPTY_UDP_ADU_SIZE + responseDataSize;

      return true;
   }

   return false;
}
