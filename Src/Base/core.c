#include "private.h"
#include "Functions/03Function/03_function.h"
#include "Functions/16Function/16_function.h"

#include "SDeviceCore/common.h"

#define EMPTY_PDU_SIZE (sizeof(MessagePdu) - SIZEOF_MEMBER(MessagePdu, Data))

typedef enum
{
   FUNCTION_CODE_READ_HOLDING_REGISTERS    = 0x03,
   FUNCTION_CODE_READ_INPUT_REGISTERS      = 0x04,
   FUNCTION_CODE_PRESET_MULTIPLE_REGISTERS = 0x10,

   FUNCTION_CODE_EXCEPTION_RESPONSE_FLAG   = 0x80
} FunctionCode;

typedef ModbusSDeviceProtocolException (* RequestFunctionProcessFunction)(SDEVICE_HANDLE(Modbus) *handle,
                                                                          const void             *operationContext,
                                                                          FunctionInput           input,
                                                                          FunctionOutput          output);

static void EncodeExceptionResponsePdu(SDEVICE_HANDLE(Modbus)        *handle,
                                       ModbusSDeviceProtocolException exception,
                                       FunctionCode                   functionCode,
                                       PduOutput                      output)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(output.Pdu != NULL);
   SDeviceDebugAssert(output.PduSize != NULL);

   *output.Pdu = (MessagePdu)
   {
      .FunctionCode = functionCode | FUNCTION_CODE_EXCEPTION_RESPONSE_FLAG,
      .Data.AsExceptionResponse =
      {
         .ExceptionCode = exception
      }
   };
   *output.PduSize = EMPTY_PDU_SIZE + SIZEOF_MEMBER(MessagePdu, Data.AsExceptionResponse);
}

bool ModbusSDeviceTryProcessRequestPdu(SDEVICE_HANDLE(Modbus) *handle,
                                       const void             *operationContext,
                                       PduInput                input,
                                       PduOutput               output)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(input.Pdu != NULL);
   SDeviceDebugAssert(output.Pdu != NULL);
   SDeviceDebugAssert(output.PduSize != NULL);

   if(input.PduSize < EMPTY_PDU_SIZE || input.PduSize > MAX_PDU_SIZE)
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST);
      return false;
   }

   RequestFunctionProcessFunction function;
   switch(input.Pdu->FunctionCode)
   {
#if MODBUS_SDEVICE_USE_FUNCTION_04_AS_FUNCTION_03_ALIAS
      case FUNCTION_CODE_READ_INPUT_REGISTERS:
         /* fall-through */
#endif
      case FUNCTION_CODE_READ_HOLDING_REGISTERS:
         function = ProcessRequest03Function;
         break;

      case FUNCTION_CODE_PRESET_MULTIPLE_REGISTERS:
         function = ProcessRequest16Function;
         break;

      default:
         SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_WRONG_FUNCTION_CODE);
         EncodeExceptionResponsePdu(handle,
                                    MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_FUNCTION,
                                    input.Pdu->FunctionCode,
                                    output);
         return true;
   }

   size_t functionResponseSize;
   ModbusSDeviceProtocolException exception =
         function(handle,
                  operationContext,
                  (FunctionInput)
                  {
                     .Request     = &input.Pdu->Data.AsFunctionRequest,
                     .RequestSize = input.PduSize - EMPTY_PDU_SIZE
                  },
                  (FunctionOutput)
                  {
                     .Response     = &output.Pdu->Data.AsFunctionResponse,
                     .ResponseSize = &functionResponseSize
                  });

   if(exception != MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK)
   {
      if(exception == MODBUS_SDEVICE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR)
         return false;

      EncodeExceptionResponsePdu(handle, exception, input.Pdu->FunctionCode, output);

      return true;
   }

   output.Pdu->FunctionCode = input.Pdu->FunctionCode;

   *output.PduSize = EMPTY_PDU_SIZE + functionResponseSize;

   return true;
}
