#include "private.h"
#include "BtuBlock/btu_block.h"

#include "SDeviceCore/heap.h"
#include "SDeviceCore/assert.h"
#include "SDeviceCore/common.h"

#include <memory.h>

#define MODBUS_UDP_MBAP_HEADER_PROTOCOL_ID 0x0000
#define MODBUS_UDP_BTU_MBAP_HEADER_PROTOCOL_ID 0x0074
#define EMPTY_UDP_ADU_SIZE sizeof(UdpAdu)

typedef struct __attribute__((packed))
{
   uint16_t TransactionIdx;
   uint16_t ProtocolIdx;
   uint16_t PacketSize;
   uint8_t  SlaveAddress;
} __attribute__((may_alias)) UdpMbapHeader;

typedef struct __attribute__((packed))
{
   UdpMbapHeader MbapHeader;
   uint8_t       PduBytes[];
} __attribute__((may_alias)) UdpAdu;

typedef bool (* RequestPduTryProcessFunction)(
      void                                 *this,
      ModbusSDevicePduProcessingStageInput  input,
      ModbusSDevicePduProcessingStageOutput output);

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusUdp, init, context)
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

SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusUdp, this)
{
   ThisHandle *_this = this;

   SDeviceAssert(_this);

   SDeviceFreeHandle(_this);
}

SDEVICE_GET_SIMPLE_PROPERTY_DECLARATION(ModbusUdp, BtuAddress, this, value)
{
   ThisHandle *_this = this;

   SDeviceAssert(_this);
   SDeviceAssert(value);

   memcpy(value, &_this->Runtime->BtuAddress, sizeof(_this->Runtime->BtuAddress));

   return SDEVICE_PROPERTY_STATUS_OK;
}

SDEVICE_SET_SIMPLE_PROPERTY_DECLARATION(ModbusUdp, BtuAddress, this, value)
{
   ThisHandle *_this = this;

   SDeviceAssert(_this);
   SDeviceAssert(value);

   memcpy(&_this->Runtime->BtuAddress, value, sizeof(_this->Runtime->BtuAddress));

   return SDEVICE_PROPERTY_STATUS_OK;
}

bool ModbusUdpSDeviceTryProcessRequest(
      ThisHandle            *this,
      ModbusUdpSDeviceInput  input,
      ModbusUdpSDeviceOutput output)
{
   SDeviceAssert(this);
   SDeviceAssert(input.RequestData);
   SDeviceAssert(output.ResponseData);
   SDeviceAssert(output.ResponseSize);

   const UdpAdu *request  = input.RequestData;
   UdpAdu *response = output.ResponseData;

   if(input.RequestSize < EMPTY_UDP_ADU_SIZE ||
         input.RequestSize - (EMPTY_UDP_ADU_SIZE - SIZEOF_MEMBER(UdpAdu, MbapHeader.SlaveAddress)) !=
               __builtin_bswap16(request->MbapHeader.PacketSize))
   {
      return false;
   }

   uint16_t protocolIdx = request->MbapHeader.ProtocolIdx;
   uint16_t transactionIdx = request->MbapHeader.TransactionIdx;
   uint8_t  slaveAddress = request->MbapHeader.SlaveAddress;

   bool isBroadcastRequest;
   RequestPduTryProcessFunction pduProcessor;
   switch(protocolIdx)
   {
      case __builtin_bswap16(MODBUS_UDP_MBAP_HEADER_PROTOCOL_ID):
         isBroadcastRequest = input.IsBroadcast;
         pduProcessor       = ModbusSDeviceInternalTryProcessRequestPdu;
         break;

      case __builtin_bswap16(MODBUS_UDP_BTU_MBAP_HEADER_PROTOCOL_ID):
         isBroadcastRequest = false;
         pduProcessor       = TryProcessRequestBtuBlock;
         break;

      default:
         return false;
   }

   size_t responseDataSize;
   bool wasProcessingSuccessful =
         pduProcessor(
               this,
               (ModbusSDevicePduProcessingStageInput)
               {
                  .RequestData       = request->PduBytes,

                  .CallParameters    = &(const ModbusUdpSDeviceCallParameters)
                  {
                     .IsBroadcast  = isBroadcastRequest,
                     .SlaveAddress = slaveAddress
                  },

                  .RequestSize       = input.RequestSize - EMPTY_UDP_ADU_SIZE,
                  .IsOutputMandatory = true
               },
               (ModbusSDevicePduProcessingStageOutput)
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
         .PacketSize     =
               __builtin_bswap16(
                     responseDataSize + SIZEOF_MEMBER(UdpAdu, MbapHeader.SlaveAddress))
      };

      *output.ResponseSize = EMPTY_UDP_ADU_SIZE + responseDataSize;

      return true;
   }

   return false;
}
