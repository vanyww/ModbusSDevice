#include "private.h"

#include "SDeviceCore/heap.h"
#include "SDeviceCore/common.h"
#include "SDeviceCore/errors.h"

#define MODBUS_TCP_MBAP_HEADER_PROTOCOL_ID 0x0000
#define EMPTY_TCP_ADU_SIZE sizeof(TcpAdu)

typedef struct __attribute__((packed))
{
   uint16_t TransactionIdx;
   uint16_t ProtocolIdx;
   uint16_t PacketSize;
   uint8_t  SlaveAddress;
} __attribute__((may_alias)) TcpMbapHeader;

_Static_assert(sizeof(TcpMbapHeader) == MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE, "Modbus-TCP MBAP header size wrong!");

typedef struct __attribute__((packed))
{
   TcpMbapHeader MbapHeader;
   uint8_t       PduBytes[];
} __attribute__((may_alias)) TcpAdu;

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusTcp, init, context)
{
   SDeviceAssert(init);

   const ThisInitData *_init = init;

   SDeviceAssert(_init->Base.ReadOperation);
   SDeviceAssert(_init->Base.WriteOperation);

   ThisHandle *instance = SDeviceAllocateHandle(sizeof(*instance->Init), sizeof(*instance->Runtime));

   instance->Context = context;
   *instance->Init = *_init;

   instance->Runtime->Base.SupportsBroadcasting = false;

   return instance;
}

SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusTcp, handlePointer)
{
   SDeviceAssert(handlePointer);

   ThisHandle **_handlePointer = handlePointer;
   ThisHandle *handle = *_handlePointer;

   SDeviceAssert(handle);

   SDeviceFreeHandle(handle);

   *_handlePointer = NULL;
}

bool ModbusTcpSDeviceTryProcessMbapHeader(
      ThisHandle *handle,
      const void *mbapHeaderData,
      size_t     *leftPacketSize)
{
   SDeviceAssert(handle);

   SDeviceAssert(mbapHeaderData);
   SDeviceAssert(leftPacketSize);

   const TcpMbapHeader *mbapHeader = mbapHeaderData;

   if(mbapHeader->ProtocolIdx != SWAP_UINT16_BYTES(MODBUS_TCP_MBAP_HEADER_PROTOCOL_ID))
      return false;

   handle->Runtime->MbapHeaderData.PacketSize = SWAP_UINT16_BYTES(mbapHeader->PacketSize);

   if(handle->Runtime->MbapHeaderData.PacketSize <= SIZEOF_MEMBER(TcpMbapHeader, SlaveAddress))
      return false;

   handle->Runtime->MbapHeaderData.SlaveAddress   = mbapHeader->SlaveAddress;
   handle->Runtime->MbapHeaderData.TransactionIdx = mbapHeader->TransactionIdx;

   *leftPacketSize = handle->Runtime->MbapHeaderData.PacketSize - SIZEOF_MEMBER(TcpMbapHeader, SlaveAddress);

   return true;
}

bool ModbusTcpSDeviceTryProcessRequest(
      ThisHandle *handle,
      ThisInput   input,
      ThisOutput  output)
{
   SDeviceAssert(handle);

   SDeviceAssert(input.RequestData);
   SDeviceAssert(output.ResponseData);
   SDeviceAssert(output.ResponseSize);

   if(input.RequestSize != handle->Runtime->MbapHeaderData.PacketSize - SIZEOF_MEMBER(TcpMbapHeader, SlaveAddress))
      return false;

   TcpAdu *response = output.ResponseData;

   size_t pduResponseSize;
   bool wasPduProcessingSuccessful =
         ModbusSDeviceBaseTryProcessRequestPdu(
               handle,
               (PduProcessingStageInput)
               {
                  .RequestData       = input.RequestData,
                  .CallParameters    = &(const ThisCallParameters)
                  {
                     .SlaveAddress = handle->Runtime->MbapHeaderData.SlaveAddress
                  },
                  .RequestSize       = input.RequestSize,
                  .IsOutputMandatory = true
               },
               (PduProcessingStageOutput)
               {
                  .ResponseData = &response->PduBytes,
                  .ResponseSize = &pduResponseSize
               });

   if(wasPduProcessingSuccessful)
   {
      response->MbapHeader = (TcpMbapHeader)
      {
         .ProtocolIdx    = SWAP_UINT16_BYTES(MODBUS_TCP_MBAP_HEADER_PROTOCOL_ID),
         .TransactionIdx = handle->Runtime->MbapHeaderData.TransactionIdx,
         .SlaveAddress   = handle->Runtime->MbapHeaderData.SlaveAddress,
         .PacketSize     = SWAP_UINT16_BYTES(pduResponseSize + SIZEOF_MEMBER(TcpAdu, MbapHeader.SlaveAddress))
      };

      *output.ResponseSize = EMPTY_TCP_ADU_SIZE + pduResponseSize;

      return true;
   }

   return false;
}
