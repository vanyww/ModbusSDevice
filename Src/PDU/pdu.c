#include "pdu.h"
#include "03Function/03_function.h"
#include "16Function/16_function.h"

#define __MODBUS_EXCEPTION_FLAG ((uint8_t)0x80)

typedef struct
{
   uint8_t FunctionCode;
   uint8_t ExceptionCode;
} ExceptionPdu;

/* function codes that are implemented */
typedef enum
{
   FUNCTION_CODE_READ_HOLDING_REGISTERS    = 0x03,
   FUNCTION_CODE_PRESET_MULTIPLE_REGISTERS = 0x10
} FunctionCode;

size_t ModbusProcessPdu(__SDEVICE_HANDLE(Modbus) *handle, ModbusCommonRequestProcessingData *processingData)
{
   ModbusCommonPdu *requestPdu = handle->Dynamic.ReceiveBufferPdu;
   ModbusSDeviceStatus (* function)(__SDEVICE_HANDLE(Modbus) *, ModbusFunctionProcessingData *, size_t *);

   switch(requestPdu->FunctionCode)
   {
      case FUNCTION_CODE_READ_HOLDING_REGISTERS:
         function = ModbusProcess03FunctionRequest;
         break;

      case FUNCTION_CODE_PRESET_MULTIPLE_REGISTERS:
         function = ModbusProcess16FunctionRequest;
         break;

      default:
         SDeviceRuntimeErrorRaised(handle, MODBUS_SDEVICE_RUNTIME_REQUEST_FUNCTION_CODE_ERROR);
         return ModbusEncodeExceptionPdu(handle, MODBUS_SDEVICE_STATUS_ILLEGAL_FUNCTION_ERROR);
   }

   size_t functionReplySize;
   ModbusSDeviceStatus status =
            function(handle,
                     &(ModbusFunctionProcessingData)
                     {
                        .FunctionSpecificDataSize = processingData->PduSize - sizeof(ModbusCommonPdu),
                        .RequestParameters = processingData->RequestParameters
                     },
                     &functionReplySize);

   if(status != MODBUS_SDEVICE_STATUS_OK)
      return ModbusEncodeExceptionPdu(handle, status);

   ExceptionPdu *replyPdu = handle->Dynamic.TransmitBufferPdu;
   replyPdu->FunctionCode = requestPdu->FunctionCode;

   return functionReplySize + sizeof(ModbusCommonPdu);
}

size_t ModbusEncodeExceptionPdu(__SDEVICE_HANDLE(Modbus) *handle, ModbusSDeviceStatus exceptionCode)
{
   ModbusCommonPdu *requestPdu = handle->Dynamic.ReceiveBufferPdu;
   ExceptionPdu *replyPdu = handle->Dynamic.TransmitBufferPdu;

   replyPdu->FunctionCode = requestPdu->FunctionCode | __MODBUS_EXCEPTION_FLAG;
   replyPdu->ExceptionCode = exceptionCode;

   return sizeof(ExceptionPdu);
}
