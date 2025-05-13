#include "private.h"
#include "Crc/crc.h"

#include "SDeviceCore/heap.h"
#include "SDeviceCore/assert.h"

#include <memory.h>

#define RTU_MAX_VALID_SLAVE_ADDRESS 247u
#define RTU_BROADCAST_REQUEST_SLAVE_ADDRESS 0u

#if MODBUS_RTU_SDEVICE_USE_PTP_ADDRESS
   #define RTU_IS_VALID_SLAVE_ADDRESS(address) (                                                                       \
      {                                                                                                                \
         __auto_type _address = (address);                                                                             \
                                                                                                                       \
         _address != RTU_BROADCAST_REQUEST_SLAVE_ADDRESS &&                                                            \
         _address != MODBUS_RTU_SDEVICE_PTP_ADDRESS      &&                                                            \
         _address <= RTU_MAX_VALID_SLAVE_ADDRESS;                                                                      \
      })
#else
   #define RTU_IS_VALID_SLAVE_ADDRESS(address) (                                                                       \
      {                                                                                                                \
         __auto_type _address = (address);                                                                             \
                                                                                                                       \
         _address != RTU_BROADCAST_REQUEST_SLAVE_ADDRESS &&                                                            \
         _address <= RTU_MAX_VALID_SLAVE_ADDRESS;                                                                      \
      })
#endif

#define RTU_ADU(pdu_size)                                                                                              \
   struct __attribute__((packed))                                                                                      \
   {                                                                                                                   \
      uint8_t  SlaveAddress;                                                                                           \
      uint8_t  PduBytes[(pdu_size)];                                                                                   \
      uint16_t Crc16;                                                                                                  \
   } __attribute__((may_alias))
#define EMPTY_RTU_ADU_SIZE sizeof(RTU_ADU(0))

typedef enum
{
   REQUEST_TYPE_NORMAL,

#if MODBUS_RTU_SDEVICE_USE_PTP_ADDRESS
   REQUEST_TYPE_PTP,
#endif

   REQUEST_TYPE_BROADCAST
} RequestType;

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusRtu, init, context)
{
   SDeviceAssert(init);

   const ThisInitData *_init = init;

   SDeviceAssert(_init->Base.ReadOperation);
   SDeviceAssert(_init->Base.WriteOperation);

   ThisHandle *instance = SDeviceAllocateHandle(sizeof(*instance->Init), sizeof(*instance->Runtime));

   instance->Context = context;
   *instance->Init = *_init;

   instance->Runtime->Base.SupportsBroadcasting = true;

   InitializeCrc16();

   return instance;
}

SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusRtu, this)
{
   ThisHandle *_this = this;

   SDeviceAssert(_this);

   SDeviceFreeHandle(_this);
}

SDEVICE_GET_SIMPLE_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, this, value)
{
   SDeviceAssert(this);

   SDeviceAssert(value);

   ThisHandle *_handle = this;
   memcpy(value, &_handle->Runtime->SlaveAddress, sizeof(_handle->Runtime->SlaveAddress));

   return SDEVICE_PROPERTY_STATUS_OK;
}

SDEVICE_SET_SIMPLE_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, this, value)
{
   SDeviceAssert(this);

   SDeviceAssert(value);

   ThisHandle *_handle = this;
   SDEVICE_PROPERTY_TYPE(ModbusRtu, SlaveAddress) address;
   memcpy(&address, value, sizeof(address));

   if(!RTU_IS_VALID_SLAVE_ADDRESS(address))
      return SDEVICE_PROPERTY_STATUS_VALIDATION_ERROR;

   _handle->Runtime->SlaveAddress = address;

   return SDEVICE_PROPERTY_STATUS_OK;
}

bool ModbusRtuSDeviceTryProcessRequest(
      ThisHandle *this,
      ThisInput   input,
      ThisOutput  output)
{
   SDeviceAssert(this);

   SDeviceAssert(input.RequestData);
   SDeviceAssert(output.ResponseData);
   SDeviceAssert(output.ResponseSize);

   if(input.RequestSize < EMPTY_RTU_ADU_SIZE)
      return false;

   const RTU_ADU(input.RequestSize - EMPTY_RTU_ADU_SIZE) *request = input.RequestData;

   RequestType requestType;
   uint8_t requestSlaveAddress = request->SlaveAddress;

   if(requestSlaveAddress == this->Runtime->SlaveAddress)
   {
      requestType = REQUEST_TYPE_NORMAL;
   }
#if MODBUS_RTU_SDEVICE_USE_PTP_ADDRESS
   else if(requestSlaveAddress == MODBUS_RTU_SDEVICE_PTP_ADDRESS)
   {
      requestType = REQUEST_TYPE_PTP;
   }
#endif
   else if(requestSlaveAddress == RTU_BROADCAST_REQUEST_SLAVE_ADDRESS)
   {
      requestType = REQUEST_TYPE_BROADCAST;
   }
   else
   {
      return false;
   }

   if(request->Crc16 != ComputeCrc16(this, request, sizeof(*request) - sizeof(request->Crc16)))
      return false;

   size_t pduResponseSize;
   bool wasPduProcessingSuccessful =
         ModbusSDeviceBaseTryProcessRequestPdu(
               this,
               (PduProcessingStageInput)
               {
                  .RequestData       = request->PduBytes,
                  .CallParameters    = &(const ThisCallParameters)
                  {
                     .Base.IsBroadcast = (requestType == REQUEST_TYPE_BROADCAST)
                  },
                  .RequestSize       = input.RequestSize - EMPTY_RTU_ADU_SIZE,
                  .IsOutputMandatory = (requestType != REQUEST_TYPE_BROADCAST)
               },
               (PduProcessingStageOutput)
               {
                  .ResponseData = ((RTU_ADU(0) *)output.ResponseData)->PduBytes,
                  .ResponseSize = &pduResponseSize
               });

   if(wasPduProcessingSuccessful)
   {
      if(requestType == REQUEST_TYPE_BROADCAST)
      {
         *output.ResponseSize = 0;
      }
      else
      {
         uint8_t responseSlaveAddress =
#if MODBUS_RTU_SDEVICE_USE_PTP_ADDRESS
               (requestType == REQUEST_TYPE_PTP) ? this->Runtime->SlaveAddress : requestSlaveAddress;
#else
               requestSlaveAddress;
#endif

         RTU_ADU(pduResponseSize) *response = output.ResponseData;

         response->SlaveAddress = responseSlaveAddress;
         response->Crc16        = ComputeCrc16(this, response, sizeof(*response) - sizeof(response->Crc16));

         *output.ResponseSize = sizeof(*response);
      }

      return true;
   }

   return false;
}
