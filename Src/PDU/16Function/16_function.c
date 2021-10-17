#include "16_function.h"

typedef struct __attribute__((scalar_storage_order("big-endian"), packed))
{
   uint16_t DataRegisterAddress;
   uint16_t RegistersToWriteCount;
   uint8_t BytesToFollow;
   ModbusSDeviceRegister RegistersBuffer[];
} ModbusFunction16RequestData;

typedef struct __attribute__((scalar_storage_order("big-endian"), packed))
{
   uint16_t DataRegisterAddress;
   uint16_t WrittenRegistersCount;
} ModbusFunction16ReplyData;

ModbusSDeviceStatus ModbusProcess16FunctionRequest(__SDEVICE_HANDLE(Modbus) *handle,
                                                   ModbusFunctionProcessingData *processingData,
                                                   size_t *replySize)
{
   if(processingData->FunctionSpecificDataSize < sizeof(ModbusFunction16RequestData))
      return MODBUS_SDEVICE_STATUS_NON_MODBUS_ERROR;

   ModbusFunction16RequestData *request =
            (ModbusFunction16RequestData *)handle->Dynamic.ReceiveBufferFunctionSpecificData;

   if(request->BytesToFollow / sizeof(ModbusSDeviceRegister) != request->RegistersToWriteCount ||
      request->BytesToFollow % sizeof(ModbusSDeviceRegister) != 0                              ||
      request->BytesToFollow != processingData->FunctionSpecificDataSize - sizeof(ModbusFunction16RequestData))
      return MODBUS_SDEVICE_STATUS_ILLEGAL_DATA_ERROR;

   ModbusSDeviceStatus status =
            handle->Constant.WriteRegistersFunction(handle,
                                                    request->RegistersBuffer,
                                                    &(ModbusSDeviceOperationParameters)
                                                    {
                                                       .RegisterAddress = request->DataRegisterAddress,
                                                       .RegistersCount = request->RegistersToWriteCount,
                                                       .RequestContext = processingData->RequestParameters
                                                    });

   if(status != MODBUS_SDEVICE_STATUS_OK)
      return status;

   ModbusFunction16ReplyData *reply = (ModbusFunction16ReplyData *)handle->Dynamic.TransmitBufferFunctionSpecificData;
   reply->DataRegisterAddress = request->DataRegisterAddress;
   reply->WrittenRegistersCount = request->RegistersToWriteCount;

   *replySize = sizeof(ModbusFunction16ReplyData);

   return MODBUS_SDEVICE_STATUS_OK;
}
