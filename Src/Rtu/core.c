#include "private.h"

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
   REQUEST_TYPE_PTP,
   REQUEST_TYPE_BROADCAST
} RequestType;

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusRtu, init, context)
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

SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusRtu, this)
{
   ThisHandle *_this = this;

   SDeviceAssert(_this);

   SDeviceFreeHandle(_this);
}

SDEVICE_GET_SIMPLE_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, this, value)
{
   ThisHandle *_this = this;

   SDeviceAssert(_this);
   SDeviceAssert(value);

   memcpy(value, &_this->Runtime->SlaveAddress, sizeof(_this->Runtime->SlaveAddress));

   return SDEVICE_PROPERTY_STATUS_OK;
}

SDEVICE_SET_SIMPLE_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, this, value)
{
   ThisHandle *_this = this;

   SDeviceAssert(_this);
   SDeviceAssert(value);

   SDEVICE_PROPERTY_TYPE(ModbusRtu, SlaveAddress) slaveAddress;
   memcpy(&slaveAddress, value, sizeof(slaveAddress));

   if(!RTU_IS_VALID_SLAVE_ADDRESS(slaveAddress))
      return SDEVICE_PROPERTY_STATUS_VALIDATION_ERROR;

   _this->Runtime->SlaveAddress = slaveAddress;

   return SDEVICE_PROPERTY_STATUS_OK;
}

bool ModbusRtuSDeviceTryProcessRequest(
      ThisHandle            *this,
      ModbusRtuSDeviceInput  input,
      ModbusRtuSDeviceOutput output)
{
   SDeviceAssert(this);

   SDeviceAssert(input.RequestData);
   SDeviceAssert(output.ResponseData);
   SDeviceAssert(output.ResponseSize);

   if(input.RequestSize < EMPTY_RTU_ADU_SIZE)
      return false;

   const RTU_ADU(input.RequestSize - EMPTY_RTU_ADU_SIZE) *request =
         input.RequestData;

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

   uint16_t expectedCrc =
         this->Init->ComputeCrc16(
               this, request, sizeof(*request) - sizeof(request->Crc16));

   if(expectedCrc != request->Crc16)
      return false;

   size_t pduResponseSize;
   bool wasPduProcessingSuccessful =
         ModbusSDeviceInternalTryProcessRequestPdu(
               this,
               (ModbusSDevicePduProcessingStageInput)
               {
                  .RequestData       = request->PduBytes,

                  .CallParameters    = &(const ModbusRtuSDeviceCallParameters)
                  {
                     .IsBroadcast = (requestType == REQUEST_TYPE_BROADCAST)
                  },

                  .RequestSize       = input.RequestSize - EMPTY_RTU_ADU_SIZE,
                  .IsOutputMandatory = (requestType != REQUEST_TYPE_BROADCAST)
               },
               (ModbusSDevicePduProcessingStageOutput)
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
         RTU_ADU(pduResponseSize) *response = output.ResponseData;

#if MODBUS_RTU_SDEVICE_USE_PTP_ADDRESS
         response->SlaveAddress =
               (requestType == REQUEST_TYPE_PTP) ?
                     this->Runtime->SlaveAddress :
                     requestSlaveAddress;
#else
         response->SlaveAddress = requestSlaveAddress;
#endif

         response->Crc16 =
               this->Init->ComputeCrc16(
                     this, response, sizeof(*response) - sizeof(response->Crc16));

         *output.ResponseSize = sizeof(*response);
      }

      return true;
   }

   return false;
}
