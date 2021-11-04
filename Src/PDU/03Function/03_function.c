#include "03_function.h"

#define __MODBUS_FUNCTION_03_MAX_REGISTERS_COUNT                                                                       \
   ((__MODBUS_MAX_PDU_SIZE - sizeof(ModbusFunction03ReplyData)) / sizeof(ModbusSDeviceRegister))

typedef struct __attribute__((scalar_storage_order("big-endian"), packed))
{
   uint16_t DataRegisterAddress;
   uint16_t RegistersToReadCount;
} ModbusFunction03RequestData;

typedef struct __attribute__((scalar_storage_order("big-endian"), packed))
{
   uint8_t FollowingDataBytes;
   ModbusSDeviceRegister RegistersBuffer[];
} ModbusFunction03ReplyData;

ModbusSDeviceStatus ModbusProcess03FunctionRequest(__SDEVICE_HANDLE(Modbus) *handle,
                                                   ModbusFunctionProcessingData *processingData,
                                                   size_t *replySize)
{
   if(processingData->FunctionSpecificDataSize != sizeof(ModbusFunction03RequestData))
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REQUEST_SIZE_ERROR);
      return MODBUS_SDEVICE_STATUS_NON_MODBUS_ERROR;
   }

   ModbusFunction03RequestData *request =
            (ModbusFunction03RequestData *)handle->Dynamic.ReceiveBufferFunctionSpecificData;

   if(request->RegistersToReadCount > __MODBUS_FUNCTION_03_MAX_REGISTERS_COUNT)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REQUEST_REGISTER_COUNT_ERROR);
      return MODBUS_SDEVICE_STATUS_ILLEGAL_DATA_ERROR;
   }

   ModbusFunction03ReplyData *reply = (ModbusFunction03ReplyData *)handle->Dynamic.TransmitBufferFunctionSpecificData;

   ModbusSDeviceStatus status =
            handle->Constant->ReadRegistersFunction(handle,
                                                    reply->RegistersBuffer,
                                                    &(ModbusSDeviceOperationParameters)
                                                    {
                                                       .RegisterAddress = request->DataRegisterAddress,
                                                       .RegistersCount = request->RegistersToReadCount,
                                                       .RequestContext = processingData->RequestParameters
                                                    });

   if(status != MODBUS_SDEVICE_STATUS_OK)
   {
      SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REGISTER_ACCESS_ERROR);
      return status;
   }

   reply->FollowingDataBytes = request->RegistersToReadCount * sizeof(ModbusSDeviceRegister);

   *replySize = sizeof(ModbusFunction03ReplyData) + reply->FollowingDataBytes;

   return MODBUS_SDEVICE_STATUS_OK;
}
