#include "private.h"
#include "Crc/crc.h"

#include "SDeviceCore/heap.h"

#include <memory.h>

#define RTU_MAX_VALID_SLAVE_ADDRESS         ((uint8_t)247)
#define RTU_BROADCAST_REQUEST_SLAVE_ADDRESS ((uint8_t)0)

#if MODBUS_RTU_SDEVICE_USE_PTP_ADDRESS
   #define RTU_IS_VALID_SLAVE_ADDRESS(address) (                                                                       \
   {                                                                                                                   \
      __auto_type _address = (address);                                                                                \
      _address != RTU_BROADCAST_REQUEST_SLAVE_ADDRESS &&                                                               \
      _address != MODBUS_RTU_SDEVICE_PTP_ADDRESS      &&                                                               \
      _address <= RTU_MAX_VALID_SLAVE_ADDRESS;                                                                         \
   })
#else
   #define RTU_IS_VALID_SLAVE_ADDRESS(address) (                                                                       \
   {                                                                                                                   \
      __auto_type _address = (address);                                                                                \
      _address != RTU_BROADCAST_REQUEST_SLAVE_ADDRESS &&                                                               \
      _address <= RTU_MAX_VALID_SLAVE_ADDRESS;                                                                         \
   })
#endif

#define RTU_ADU(pdu_size)                                                                                              \
struct __attribute__((packed))                                                                                         \
{                                                                                                                      \
   uint8_t  SlaveAddress;                                                                                              \
   uint8_t  PduBytes[(pdu_size)];                                                                                      \
   uint16_t Crc16;                                                                                                     \
}
#define EMPTY_RTU_ADU_SIZE sizeof(RTU_ADU(0))


SDEVICE_IDENTITY_BLOCK_DEFINITION(ModbusRtu,
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
   SDeviceAssert(init != NULL);

   const ThisInitData *_init = init;

   SDeviceAssert(_init->BaseInit.ReadOperation != NULL);
   SDeviceAssert(_init->BaseInit.WriteOperation != NULL);

   ThisHandle *handle = SDeviceAllocHandle(sizeof(ThisInitData), sizeof(ThisRuntimeData));
   handle->Header = (SDeviceHandleHeader)
   {
      .Context       = context,
      .OwnerHandle   = owner,
      .IdentityBlock = &SDEVICE_IDENTITY_BLOCK(ModbusRtu),
      .LatestStatus  = MODBUS_RTU_SDEVICE_STATUS_OK,
      .Identifier    = identifier
   };
   *handle->Init = *_init;

   InitializeCrc16();

   return handle;
}

SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusRtu, handlePointer)
{
   SDeviceAssert(handlePointer != NULL);

   ThisHandle **_handlePointer = handlePointer;
   ThisHandle *handle = *_handlePointer;

   SDeviceFreeHandle(handle);
   *_handlePointer = NULL;
}

SDEVICE_GET_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, handle, value)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(value != NULL);

   ThisHandle *_handle = handle;
   memcpy(value, &_handle->Runtime->SlaveAddress, sizeof(_handle->Runtime->SlaveAddress));

   return SDEVICE_PROPERTY_STATUS_OK;
}

SDEVICE_SET_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, handle, value)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(value != NULL);

   ThisHandle *_handle = handle;
   typeof(_handle->Runtime->SlaveAddress) address;
   memcpy(&address, value, sizeof(_handle->Runtime->SlaveAddress));

   if(!RTU_IS_VALID_SLAVE_ADDRESS(address))
   {
      SDeviceLogStatus(handle, MODBUS_RTU_SDEVICE_STATUS_SLAVE_ADDRESS_SET_INVALID);
      return SDEVICE_PROPERTY_STATUS_VALIDATION_ERROR;
   }

   _handle->Runtime->SlaveAddress = address;

   return SDEVICE_PROPERTY_STATUS_OK;
}

bool ModbusRtuSDeviceTryProcessRequest(SDEVICE_HANDLE(ModbusRtu) *handle,
                                       ModbusRtuSDeviceInput      input,
                                       ModbusRtuSDeviceOutput     output)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(input.RequestData != NULL);
   SDeviceAssert(output.ResponseData != NULL);
   SDeviceAssert(output.ResponseSize != NULL);

   if(input.RequestSize < EMPTY_RTU_ADU_SIZE)
   {
      SDeviceLogStatus(handle, MODBUS_RTU_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return false;
   }

   const RTU_ADU(input.RequestSize - EMPTY_RTU_ADU_SIZE) *request = input.RequestData;
   ModbusRtuSDeviceOperationContext context;

#if MODBUS_RTU_SDEVICE_USE_PTP_ADDRESS
   if(request->SlaveAddress == handle->Runtime->SlaveAddress || request->SlaveAddress == MODBUS_RTU_SDEVICE_PTP_ADDRESS)
#else
   if(request->SlaveAddress == handle->Runtime->SlaveAddress)
#endif
   {
      context.IsBroadcast = false;
   }
   else
   {
      if(request->SlaveAddress != RTU_BROADCAST_REQUEST_SLAVE_ADDRESS)
         return false;

      context.IsBroadcast = true;
   }

   if(request->Crc16 != ComputeCrc16(handle, request, sizeof(*request) - sizeof(request->Crc16)))
   {
      SDeviceLogStatus(handle, MODBUS_RTU_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return false;
   }

   size_t pduResponseSize;
   bool wasPduProcessingSuccessful =
         ModbusSDeviceTryProcessRequestPdu((SDEVICE_HANDLE(Modbus) *)handle,
                                           &context,
                                           (PduInput)
                                           {
                                              .Pdu     = (const MessagePdu *)request->PduBytes,
                                              .PduSize = input.RequestSize - EMPTY_RTU_ADU_SIZE
                                           },
                                           (PduOutput)
                                           {
                                              .Pdu     = (MessagePdu *)((RTU_ADU(0) *)output.ResponseData)->PduBytes,
                                              .PduSize = &pduResponseSize
                                           });

   if(wasPduProcessingSuccessful)
   {
      if(context.IsBroadcast)
      {
         *output.ResponseSize = 0;
      }
      else
      {
         RTU_ADU(pduResponseSize) *response = output.ResponseData;
         response->SlaveAddress = handle->Runtime->SlaveAddress;
         response->Crc16 = ComputeCrc16(handle, response, sizeof(*response) - sizeof(response->Crc16));

         *output.ResponseSize = sizeof(*response);
      }
   }

   return wasPduProcessingSuccessful;
}
