#include "private.h"

#include "SDeviceCore/heap.h"
#include "SDeviceCore/assert.h"
#include "SDeviceCore/common.h"

#define MODBUS_TCP_MBAP_HEADER_PROTOCOL_ID 0x0000
#define EMPTY_TCP_ADU_SIZE sizeof(TcpAdu)

typedef struct __attribute__((packed))
{
   uint16_t TransactionIdx;
   uint16_t ProtocolIdx;
   uint16_t PacketSize;
   uint8_t  SlaveAddress;
} __attribute__((may_alias)) TcpMbapHeader;

static_assert(sizeof(TcpMbapHeader) == MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE);

typedef struct __attribute__((packed))
{
   TcpMbapHeader MbapHeader;
   uint8_t       PduBytes[];
} __attribute__((may_alias)) TcpAdu;

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusTcp, init, context)
{
   const ThisInitData *_init = init;

   SDeviceAssert(_init);
   SDeviceAssert(_init->Base.ReadOperation);
   SDeviceAssert(_init->Base.WriteOperation);

   ThisHandle *instance =
         SDeviceAllocateHandle(
               sizeof(*instance->Init),
               sizeof(*instance->Runtime));

   *instance->Init = *_init;

   instance->Context = context;

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
      const void *MbapHeader,
      size_t     *leftPacketSize)
{
   SDeviceAssert(this);
   SDeviceAssert(MbapHeader);
   SDeviceAssert(leftPacketSize);

   const TcpMbapHeader *mbapHeader = MbapHeader;

   if(mbapHeader->ProtocolIdx != __builtin_bswap16(MODBUS_TCP_MBAP_HEADER_PROTOCOL_ID))
      return false;

   this->Runtime->MbapHeader.PacketSize = __builtin_bswap16(mbapHeader->PacketSize);

   if(this->Runtime->MbapHeader.PacketSize <= SIZEOF_MEMBER(TcpMbapHeader, SlaveAddress))
      return false;

   this->Runtime->MbapHeader.SlaveAddress = mbapHeader->SlaveAddress;
   this->Runtime->MbapHeader.TransactionIdx = mbapHeader->TransactionIdx;

   *leftPacketSize =
         this->Runtime->MbapHeader.PacketSize -
         SIZEOF_MEMBER(TcpMbapHeader, SlaveAddress);

   return true;
}

bool ModbusTcpSDeviceTryProcessRequest(
      ThisHandle            *this,
      ModbusTcpSDeviceInput  input,
      ModbusTcpSDeviceOutput output)
{
   SDeviceAssert(this);
   SDeviceAssert(input.RequestData);
   SDeviceAssert(output.ResponseData);
   SDeviceAssert(output.ResponseSize);

   size_t expectedRequestSize =
         this->Runtime->MbapHeader.PacketSize -
         SIZEOF_MEMBER(TcpMbapHeader, SlaveAddress);

   if(expectedRequestSize != input.RequestSize)
      return false;

   TcpAdu *response = output.ResponseData;

   size_t pduResponseSize;
   bool wasPduProcessingSuccessful =
         ModbusSDeviceInternalTryProcessRequestPdu(
               this,
               (ModbusSDevicePduProcessingStageInput)
               {
                  .RequestData       = input.RequestData,

                  .CallParameters    = &(const ModbusTcpSDeviceCallParameters)
                  {
                     .SlaveAddress = this->Runtime->MbapHeader.SlaveAddress
                  },

                  .RequestSize       = input.RequestSize,
                  .IsOutputMandatory = true
               },
               (ModbusSDevicePduProcessingStageOutput)
               {
                  .ResponseData = &response->PduBytes,
                  .ResponseSize = &pduResponseSize
               });

   if(wasPduProcessingSuccessful)
   {
      response->MbapHeader = (TcpMbapHeader)
      {
         .ProtocolIdx    = __builtin_bswap16(MODBUS_TCP_MBAP_HEADER_PROTOCOL_ID),
         .TransactionIdx = this->Runtime->MbapHeader.TransactionIdx,
         .SlaveAddress   = this->Runtime->MbapHeader.SlaveAddress,
         .PacketSize     =
               __builtin_bswap16(
                     pduResponseSize + SIZEOF_MEMBER(TcpAdu, MbapHeader.SlaveAddress))
      };

      *output.ResponseSize = EMPTY_TCP_ADU_SIZE + pduResponseSize;

      return true;
   }

   return false;
}
