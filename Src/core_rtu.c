#include "PDU/pdu.h"
#include "Crc/crc.h"

#include "SDeviceCore/heap.h"

#include <memory.h>

#define MODBUS_RTU_IS_VALID_SLAVE_ADDRESS(address) (                                                                   \
{                                                                                                                      \
   __auto_type _address = (address);                                                                                   \
   _address != MODBUS_RTU_SDEVICE_BROADCAST_REQUEST_SLAVE_ADDRESS &&                                                   \
   _address <= MODBUS_RTU_SDEVICE_MAX_VALID_SLAVE_ADDRESS;                                                             \
})

#define MODBUS_RTU_EMPTY_ADU_SIZE (sizeof(MODBUS_RTU_ADU(0)))
#define MODBUS_RTU_ADU(pdu_size)                                                                                       \
struct __attribute__((packed))                                                                                         \
{                                                                                                                      \
   uint8_t SlaveAddress;                                                                                               \
   uint8_t PduBytes[(pdu_size)];                                                                                       \
   uint16_t Crc16;                                                                                                     \
}

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusRtu, init, parent, identifier, context)
{
   SDeviceAssert(init != NULL);

   const SDEVICE_INIT_DATA(ModbusRtu) *_init = init;

   SDeviceAssert(_init->RegistersCallbacks.ReadRegisters != NULL);
   SDeviceAssert(_init->RegistersCallbacks.WriteRegisters != NULL);

   SDEVICE_HANDLE(ModbusRtu) *handle = SDeviceMalloc(sizeof(SDEVICE_HANDLE(ModbusRtu)));

   SDeviceAssert(handle != NULL);

   handle->Init = *_init;
   handle->Header = (SDeviceHandleHeader)
   {
      .Context = context,
      .ParentHandle = parent,
      .Identifier = identifier,
      .LatestStatus = MODBUS_RTU_SDEVICE_STATUS_OK
   };

   InitializeCrc16();

   return handle;
}

SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusRtu, handlePointer)
{
   SDeviceAssert(handlePointer != NULL);

   SDEVICE_HANDLE(ModbusRtu) **_handlePointer = handlePointer;
   SDEVICE_HANDLE(ModbusRtu) *handle = *_handlePointer;

   SDeviceAssert(handle != NULL);

   SDeviceFree(handle);
   *_handlePointer = NULL;
}

SDEVICE_GET_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, handle, value)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(value != NULL);

   SDEVICE_HANDLE(ModbusRtu) *_handle = handle;
   memcpy(value, &_handle->Runtime.SlaveAddress, sizeof(_handle->Runtime.SlaveAddress));

   return SDEVICE_PROPERTY_OPERATION_STATUS_OK;
}

SDEVICE_SET_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, handle, value)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(value != NULL);

   SDEVICE_HANDLE(ModbusRtu) *_handle = handle;
   typeof(_handle->Runtime.SlaveAddress) address;
   memcpy(&address, value, sizeof(_handle->Runtime.SlaveAddress));

   if(MODBUS_RTU_IS_VALID_SLAVE_ADDRESS(address) != true)
   {
      SDeviceLogStatus(handle, MODBUS_RTU_SDEVICE_STATUS_SLAVE_ADDRESS_INVALID_SET);
      return SDEVICE_PROPERTY_OPERATION_STATUS_VALIDATION_ERROR;
   }

   _handle->Runtime.SlaveAddress = address;

   return SDEVICE_PROPERTY_OPERATION_STATUS_OK;
}

bool ModbusRtuSDeviceTryProcessRequest(SDEVICE_HANDLE(ModbusRtu) *handle,
                                       const ModbusSDeviceRequest *request,
                                       ModbusSDeviceResponse *response)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(request != NULL);
   SDeviceAssert(response != NULL);
   SDeviceAssert(request->Data != NULL);
   SDeviceAssert(response->Data != NULL);

   if(request->Size < MODBUS_RTU_EMPTY_ADU_SIZE || request->Size > MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE)
   {
      SDeviceLogStatus(handle, MODBUS_RTU_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return false;
   }

   ModbusRtuSDeviceRequestContext requestContext = { .Common.ProtocolType = MODBUS_SDEVICE_PROTOCOL_TYPE_RTU };
   const MODBUS_RTU_ADU(request->Size - MODBUS_RTU_EMPTY_ADU_SIZE) *requestAdu = request->Data;

   if(requestAdu->SlaveAddress != handle->Runtime.SlaveAddress)
   {
      if(requestAdu->SlaveAddress != MODBUS_RTU_SDEVICE_BROADCAST_REQUEST_SLAVE_ADDRESS)
         return false;

      requestContext.RequestType = MODBUS_RTU_SDEVICE_REQUEST_TYPE_BROADCAST;
   }
   else
   {
      requestContext.RequestType = MODBUS_RTU_SDEVICE_REQUEST_TYPE_NORMAL;
   }

   if(requestAdu->Crc16 != ComputeCrc16(handle, requestAdu, sizeof(*requestAdu) - sizeof(requestAdu->Crc16)))
   {
      SDeviceLogStatus(handle, MODBUS_RTU_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return false;
   }

   ModbusSDeviceRequest requestPdu = { requestAdu->PduBytes, sizeof(requestAdu->PduBytes) };
   ModbusSDeviceResponse responsePdu = { ((MODBUS_RTU_ADU(0) *)response->Data)->PduBytes };
   const ModbusProcessingParameters processingParameters =
   {
      .RegistersCallbacks = &handle->Init.RegistersCallbacks,
      .RequestContext = &requestContext
   };

   if(TryProcessRequestPdu(handle, &processingParameters, &requestPdu, &responsePdu) != true)
      return false;

   if(requestContext.RequestType == MODBUS_RTU_SDEVICE_REQUEST_TYPE_BROADCAST)
   {
      response->Size = 0;
      return true;
   }

   MODBUS_RTU_ADU(responsePdu.Size) *responseAdu = response->Data;
   responseAdu->SlaveAddress = handle->Runtime.SlaveAddress;
   responseAdu->Crc16 = ComputeCrc16(handle, responseAdu, sizeof(*responseAdu) - sizeof(responseAdu->Crc16));

   response->Size = sizeof(*responseAdu);

   return true;
}
