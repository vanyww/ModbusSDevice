#include "private_rtu.h"
#include "CRC/modbus_crc.h"
#include "PDU/pdu.h"

#include <memory.h>
#include <stdlib.h>

#define __RTU_EMPTY_ADU_SIZE (sizeof(__RTU_ADU(0)))
#define __RTU_ADU(pdu_size)                                                                                            \
struct __attribute__((packed))                                                                                         \
{                                                                                                                      \
   uint8_t SlaveAddress;                                                                                               \
   uint8_t PduBytes[(pdu_size)];                                                                                       \
   ModbusRtuCrcType Crc16;                                                                                             \
}

/**********************************************************************************************************************/

__SDEVICE_CREATE_HANDLE_DECLARATION(ModbusRtu, arguments, instanceIndex, context)
{
   SDeviceAssert(arguments != NULL);
   SDeviceAssert(arguments->Common.ReadRegisters != NULL);
   SDeviceAssert(arguments->Common.WriteRegisters != NULL);
#ifdef __MODBUS_SDEVICE_USE_EXTERN_CRC
   SDeviceAssert(arguments->ComputeCrc16 != NULL);
#endif

   __SDEVICE_HANDLE(ModbusRtu) handle =
   {
      .Init = *arguments,
      .Runtime = __SDEVICE_MALLOC(sizeof(__SDEVICE_RUNTIME_DATA(ModbusRtu))),
      .Context = context,
      .InstanceIndex = instanceIndex,
      .IsInitialized = true
   };

   return handle;
}

__SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusRtu, handle)
{
   __SDEVICE_FREE(handle->Runtime);
   handle->Runtime = NULL;
}

/* Slave address ******************************************************************************************************/

__SDEVICE_SET_PARAMETER_DECLARATION(ModbusRtu, SlaveAddress, handle, value)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(value != NULL);
   SDeviceAssert(((SDeviceCommonHandle *)handle)->IsInitialized == true);

   __SDEVICE_HANDLE(ModbusRtu) *device = handle;
   __typeof__(device->Runtime->SlaveAddress) address;
   memcpy(&address, value, sizeof(device->Runtime->SlaveAddress));

   if(__MODBUS_RTU_IS_VALID_SLAVE_ADDRESS(address) != true)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_RUNTIME_ERROR_SLAVE_ADDRESS_INVALID_SET);
      return SDEVICE_OPERATION_STATUS_VALIDATION_ERROR;
   }

   device->Runtime->SlaveAddress = address;

   return SDEVICE_OPERATION_STATUS_OK;
}

__SDEVICE_GET_PARAMETER_DECLARATION(ModbusRtu, SlaveAddress, handle, value)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(value != NULL);
   SDeviceAssert(((SDeviceCommonHandle *)handle)->IsInitialized == true);

   __SDEVICE_HANDLE(ModbusRtu) *device = handle;
   memcpy(value, &device->Runtime->SlaveAddress, sizeof(device->Runtime->SlaveAddress));

   return SDEVICE_OPERATION_STATUS_OK;
}

/**********************************************************************************************************************/

/**********************************************************************************************************************/

bool ModbusRtuTryProcessRequest(__SDEVICE_HANDLE(ModbusRtu) *handle, ModbusRequest *request, ModbusResponse *response)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(request != NULL);
   SDeviceAssert(response != NULL);
   SDeviceAssert(handle->IsInitialized == true);

   if(request->Size < __RTU_EMPTY_ADU_SIZE || request->Size > __MODBUS_RTU_MAX_MESSAGE_SIZE)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_RUNTIME_ERROR_WRONG_REQUEST_SIZE);
      return false;
   }

   ModbusRtuRequestType requestType;
   const __RTU_ADU(request->Size - __RTU_EMPTY_ADU_SIZE) *requestAdu = request->Bytes;
   if(requestAdu->SlaveAddress != handle->Runtime->SlaveAddress)
   {
      if(requestAdu->SlaveAddress != __MODBUS_RTU_BROADCAST_REQUEST_SLAVE_ADDRESS)
         return false;

      requestType = MODBUS_RTU_REQUEST_TYPE_BROADCAST;
   }
   else
   {
      requestType = MODBUS_RTU_REQUEST_TYPE_NORMAL;
   }

   if(requestAdu->Crc16 != ComputeModbusRtuCrc(handle, request->Bytes, request->Size - sizeof(ModbusRtuCrcType)))
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_RUNTIME_ERROR_REQUEST_CRC_MISMATCH);
      return false;
   }

   ModbusRequest requestPdu = { requestAdu->PduBytes, sizeof(requestAdu->PduBytes) };
   ModbusResponse responsePdu = { ((__RTU_ADU(0) *)response->Bytes)->PduBytes };
   ModbusProcessingParameters processingParameters =
   {
      .RequestContext = &(ModbusRtuRequestContext)
      {
         .Common.ModbusType = MODBUS_MODBUS_TYPE_RTU,
         .RequestType = requestType
      }
   };

   if(TryProcessModbusPdu((SDeviceCommonHandle *)handle, processingParameters, &requestPdu, &responsePdu) != true)
      return false;

   if(requestType == MODBUS_RTU_REQUEST_TYPE_BROADCAST)
   {
      response->Size = 0;
      return true;
   }

   __RTU_ADU(responsePdu.Size) *responseAdu = response->Bytes;
   responseAdu->SlaveAddress = handle->Runtime->SlaveAddress;
   responseAdu->Crc16 = ComputeModbusRtuCrc(handle, response->Bytes, sizeof(*responseAdu) - sizeof(ModbusRtuCrcType));

   response->Size = sizeof(*responseAdu);
   return true;
}
