#include "private_rtu.h"
#include "../Base/internal_base.h"
#include "Crc/crc.h"

#include "SDeviceCore/heap.h"
#include "SDeviceCore/errors.h"

#include <memory.h>

#define RTU_MAX_VALID_SLAVE_ADDRESS         ((uint8_t)247)
#define RTU_BROADCAST_REQUEST_SLAVE_ADDRESS ((uint8_t)0)
#define RTU_IS_VALID_SLAVE_ADDRESS(address) (                                                                          \
{                                                                                                                      \
   __auto_type _address = (address);                                                                                   \
   _address != RTU_BROADCAST_REQUEST_SLAVE_ADDRESS &&                                                                  \
   _address <= RTU_MAX_VALID_SLAVE_ADDRESS;                                                                            \
})

#define RTU_ADU(pdu_size)                                                                                              \
struct __attribute__((packed))                                                                                         \
{                                                                                                                      \
   uint8_t  SlaveAddress;                                                                                              \
   uint8_t  PduBytes[(pdu_size)];                                                                                      \
   uint16_t Crc16;                                                                                                     \
}
#define EMPTY_RTU_ADU_SIZE sizeof(RTU_ADU(0))

SDEVICE_STRING_NAME_DEFINITION(ModbusRtu);

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusRtu, init, owner, identifier, context)
{
   SDeviceAssert(init != NULL);

   const ThisInitData *_init = init;

   SDeviceAssert(_init->BaseInit.ReadOperation != NULL);
   SDeviceAssert(_init->BaseInit.WriteOperation != NULL);

   ThisHandle *handle = SDeviceMalloc(sizeof(ThisHandle));
   handle->Header = (SDeviceHandleHeader)
   {
      .Context           = context,
      .OwnerHandle       = owner,
      .SDeviceStringName = SDEVICE_STRING_NAME(ModbusRtu),
      .LatestStatus      = MODBUS_RTU_SDEVICE_STATUS_OK,
      .Identifier        = identifier
   };
   handle->Init = *_init;
   handle->Runtime = (ThisRuntimeData)
   {
      .BaseHandle = SDEVICE_CREATE_HANDLE(Modbus)(&_init->BaseInit, handle, 0, NULL),
   };

   InitializeCrc16();

   return handle;
}

SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusRtu, handlePointer)
{
   SDeviceAssert(handlePointer != NULL);

   ThisHandle **_handlePointer = handlePointer;
   ThisHandle *handle = *_handlePointer;

   SDeviceAssert(handle != NULL);

   SDEVICE_DISPOSE_HANDLE(Modbus)(&handle->Runtime.BaseHandle);

   SDeviceFree(handle);
   *_handlePointer = NULL;
}

SDEVICE_GET_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, handle, value)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(value != NULL);

   ThisHandle *_handle = handle;
   memcpy(value, &_handle->Runtime.SlaveAddress, sizeof(_handle->Runtime.SlaveAddress));

   return SDEVICE_PROPERTY_OPERATION_STATUS_OK;
}

SDEVICE_SET_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, handle, value)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(value != NULL);

   ThisHandle *_handle = handle;
   typeof(_handle->Runtime.SlaveAddress) address;
   memcpy(&address, value, sizeof(_handle->Runtime.SlaveAddress));

   if(!RTU_IS_VALID_SLAVE_ADDRESS(address))
   {
      SDeviceLogStatus(handle, MODBUS_RTU_SDEVICE_STATUS_SLAVE_ADDRESS_SET_INVALID);
      return SDEVICE_PROPERTY_OPERATION_STATUS_VALIDATION_ERROR;
   }

   _handle->Runtime.SlaveAddress = address;

   return SDEVICE_PROPERTY_OPERATION_STATUS_OK;
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

   if(request->SlaveAddress != handle->Runtime.SlaveAddress)
   {
      if(request->SlaveAddress != RTU_BROADCAST_REQUEST_SLAVE_ADDRESS)
         return false;

      context.IsBroadcast = true;
   }
   else
   {
      context.IsBroadcast = false;
   }

   if(request->Crc16 != ComputeCrc16(handle, request, sizeof(*request) - sizeof(request->Crc16)))
   {
      SDeviceLogStatus(handle, MODBUS_RTU_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return false;
   }

   size_t pduResponseSize;
   if(!ModbusSDeviceTryProcessRequestPdu(handle->Runtime.BaseHandle,
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
                                         }))
   {
      return false;
   }

   if(context.IsBroadcast)
   {
      *output.ResponseSize = 0;
      return true;
   }

   RTU_ADU(pduResponseSize) *response = output.ResponseData;
   response->SlaveAddress = handle->Runtime.SlaveAddress;
   response->Crc16 = ComputeCrc16(handle, response, sizeof(*response) - sizeof(response->Crc16));

   *output.ResponseSize = sizeof(*response);

   return true;
}
