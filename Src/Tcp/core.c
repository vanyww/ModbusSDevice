#include "private.h"

#include "SDeviceCore/heap.h"
#include "SDeviceCore/common.h"
#include "SDeviceCore/assert.h"

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

SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusTcp, this)
{
   ThisHandle *_this = this;

   SDeviceAssert(_this);

   SDeviceFreeHandle(_this);
}

bool ModbusTcpSDeviceTryProcessMbapHeader(
      ThisHandle *this,
      const void *mbapHeaderData,
      size_t     *leftPacketSize)
{
   SDeviceAssert(this);

   SDeviceAssert(mbapHeaderData);
   SDeviceAssert(leftPacketSize);

   const TcpMbapHeader *mbapHeader = mbapHeaderData;

   if(mbapHeader->ProtocolIdx != SWAP_UINT16_BYTES(MODBUS_TCP_MBAP_HEADER_PROTOCOL_ID))
      return false;

   this->Runtime->MbapHeaderData.PacketSize = SWAP_UINT16_BYTES(mbapHeader->PacketSize);

   if(this->Runtime->MbapHeaderData.PacketSize <= SIZEOF_MEMBER(TcpMbapHeader, SlaveAddress))
      return false;

   this->Runtime->MbapHeaderData.SlaveAddress   = mbapHeader->SlaveAddress;
   this->Runtime->MbapHeaderData.TransactionIdx = mbapHeader->TransactionIdx;

   *leftPacketSize = this->Runtime->MbapHeaderData.PacketSize - SIZEOF_MEMBER(TcpMbapHeader, SlaveAddress);

   return true;
}

bool ModbusTcpSDeviceTryProcessRequest(
      ThisHandle *this,
      ThisInput   input,
      ThisOutput  output)
{
   SDeviceAssert(this);

   SDeviceAssert(input.RequestData);
   SDeviceAssert(output.ResponseData);
   SDeviceAssert(output.ResponseSize);

   if(input.RequestSize != this->Runtime->MbapHeaderData.PacketSize - SIZEOF_MEMBER(TcpMbapHeader, SlaveAddress))
      return false;

   TcpAdu *response = output.ResponseData;

   size_t pduResponseSize;
   bool wasPduProcessingSuccessful =
         ModbusSDeviceBaseTryProcessRequestPdu(
               this,
               (PduProcessingStageInput)
               {
                  .RequestData       = input.RequestData,
                  .CallParameters    = &(const ThisCallParameters)
                  {
                     .SlaveAddress = this->Runtime->MbapHeaderData.SlaveAddress
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
         .TransactionIdx = this->Runtime->MbapHeaderData.TransactionIdx,
         .SlaveAddress   = this->Runtime->MbapHeaderData.SlaveAddress,
         .PacketSize     = SWAP_UINT16_BYTES(pduResponseSize + SIZEOF_MEMBER(TcpAdu, MbapHeader.SlaveAddress))
      };

      *output.ResponseSize = EMPTY_TCP_ADU_SIZE + pduResponseSize;

      return true;
   }

   return false;
}
