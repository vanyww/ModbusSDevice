#include "private.h"
#include "Crc/crc.h"

#include "SDeviceCore/heap.h"

#include <memory.h>

#define RTU_MAX_VALID_SLAVE_ADDRESS 247U
#define RTU_BROADCAST_REQUEST_SLAVE_ADDRESS 0U

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

SDEVICE_IDENTITY_BLOCK_DEFINITION(
      ModbusRtu,
      ((const SDeviceUuid)
      {
         .High = MODBUS_RTU_SDEVICE_UUID_HIGH,
         .Low  = MODBUS_RTU_SDEVICE_UUID_LOW
      }),
      ((const SDeviceVersion)
      {
         .Major = MODBUS_RTU_SDEVICE_VERSION_MAJOR,
         .Minor = MODBUS_RTU_SDEVICE_VERSION_MINOR,
         .Patch = MODBUS_RTU_SDEVICE_VERSION_PATCH
      }));

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusRtu, init, owner, identifier, context)
{
   SDeviceAssert(init);

   const ThisInitData *_init = init;

   SDeviceAssert(_init->Base.ReadOperation);
   SDeviceAssert(_init->Base.WriteOperation);

   ThisHandle *instance = SDeviceAllocateHandle(sizeof(*instance->Init), sizeof(*instance->Runtime));

   instance->Header = (SDeviceHandleHeader)
   {
      .Context       = context,
      .OwnerHandle   = owner,
      .IdentityBlock = &SDEVICE_IDENTITY_BLOCK(ModbusRtu),
      .LatestStatus  = MODBUS_RTU_SDEVICE_STATUS_OK,
      .Identifier    = identifier
   };

   *instance->Init = *_init;

   instance->Runtime->Base.SupportsBroadcasting = true;

   InitializeCrc16();

   return instance;
}

SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusRtu, handlePointer)
{
   SDeviceAssert(handlePointer);

   ThisHandle **_handlePointer = handlePointer;
   ThisHandle *handle = *_handlePointer;

   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   SDeviceFreeHandle(handle);

   *_handlePointer = NULL;
}

SDEVICE_GET_SIMPLE_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, handle, value)
{
   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   SDeviceAssert(value);

   ThisHandle *_handle = handle;
   memcpy(value, &_handle->Runtime->SlaveAddress, sizeof(_handle->Runtime->SlaveAddress));

   return SDEVICE_PROPERTY_STATUS_OK;
}

SDEVICE_SET_SIMPLE_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, handle, value)
{
   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   SDeviceAssert(value);

   ThisHandle *_handle = handle;
   SDEVICE_PROPERTY_TYPE(ModbusRtu, SlaveAddress) address;
   memcpy(&address, value, sizeof(address));

   if(!RTU_IS_VALID_SLAVE_ADDRESS(address))
   {
      SDeviceLogStatus(handle, MODBUS_RTU_SDEVICE_STATUS_SLAVE_ADDRESS_SET_INVALID);
      return SDEVICE_PROPERTY_STATUS_VALIDATION_ERROR;
   }

   _handle->Runtime->SlaveAddress = address;

   return SDEVICE_PROPERTY_STATUS_OK;
}

bool ModbusRtuSDeviceTryProcessRequest(
      ThisHandle *handle,
      ThisInput   input,
      ThisOutput  output)
{
   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   SDeviceAssert(input.RequestData);
   SDeviceAssert(output.ResponseData);
   SDeviceAssert(output.ResponseSize);

   if(input.RequestSize < EMPTY_RTU_ADU_SIZE)
   {
      SDeviceLogStatus(handle, MODBUS_RTU_SDEVICE_STATUS_WRONG_REQUEST_SIZE);
      return false;
   }

   const RTU_ADU(input.RequestSize - EMPTY_RTU_ADU_SIZE) *request = input.RequestData;

   bool isBroadcastRequest;
   uint8_t slaveAddress = request->SlaveAddress;

#if MODBUS_RTU_SDEVICE_USE_PTP_ADDRESS
   if(slaveAddress == handle->Runtime->SlaveAddress || slaveAddress == MODBUS_RTU_SDEVICE_PTP_ADDRESS)
#else
   if(slaveAddress == handle->Runtime->SlaveAddress)
#endif
   {
      isBroadcastRequest = false;
   }
   else if(slaveAddress == RTU_BROADCAST_REQUEST_SLAVE_ADDRESS)
   {
      isBroadcastRequest = true;
   }
   else
   {
      return false;
   }

   if(request->Crc16 != ComputeCrc16(handle, request, sizeof(*request) - sizeof(request->Crc16)))
   {
      SDeviceLogStatus(handle, MODBUS_RTU_SDEVICE_STATUS_WRONG_REQUEST_CRC);
      return false;
   }

   size_t pduResponseSize;
   bool wasPduProcessingSuccessful =
         ModbusSDeviceBaseTryProcessRequestPdu(
               handle,
               (PduProcessingStageInput)
               {
                  .RequestData       = request->PduBytes,
                  .OperationContext  = &(const ThisOperationContext)
                  {
                     .Base.IsBroadcast = isBroadcastRequest
                  },
                  .RequestSize       = input.RequestSize - EMPTY_RTU_ADU_SIZE,
                  .IsOutputMandatory = !isBroadcastRequest
               },
               (PduProcessingStageOutput)
               {
                  .ResponseData = ((RTU_ADU(0) *)output.ResponseData)->PduBytes,
                  .ResponseSize = &pduResponseSize
               });

   if(wasPduProcessingSuccessful)
   {
      if(isBroadcastRequest)
      {
         *output.ResponseSize = 0;
      }
      else
      {
         RTU_ADU(pduResponseSize) *response = output.ResponseData;

         response->SlaveAddress = (isBroadcastRequest) ? handle->Runtime->SlaveAddress : slaveAddress;
         response->Crc16        = ComputeCrc16(handle, response, sizeof(*response) - sizeof(response->Crc16));

         *output.ResponseSize = sizeof(*response);
      }

      return true;
   }

   return false;
}
