#include "private.h"
#include "Functions/exception.h"
#include "Functions/03_function.h"
#include "Functions/16_function.h"

#define EMPTY_PDU_SIZE sizeof(MessagePdu)

typedef struct __attribute__((packed))
{
   uint8_t FunctionCode;
   uint8_t FunctionData[];
} __attribute__((may_alias)) MessagePdu;

typedef enum
{
   FUNCTION_CODE_READ_HOLDING_REGISTERS    = 0x03,
   FUNCTION_CODE_READ_INPUT_REGISTERS      = 0x04,
   FUNCTION_CODE_PRESET_MULTIPLE_REGISTERS = 0x10,

   FUNCTION_CODE_EXCEPTION_RESPONSE_FLAG   = 0x80
} FunctionCode;

typedef ModbusSDeviceProtocolException (* RequestFunctionProcess)(
      void                                 *this,
      ModbusSDevicePduProcessingStageInput  input,
      ModbusSDevicePduProcessingStageOutput output);

static void EncodeExceptionResponsePdu(
      ModbusSDeviceProtocolException        exception,
      FunctionCode                          functionCode,
      ModbusSDevicePduProcessingStageOutput output)
{
   MessagePdu *response = output.ResponseData;

   size_t functionResponseSize;
   ProcessExceptionFunction(
         exception,
         (ModbusSDevicePduProcessingStageOutput)
         {
            .ResponseData = response->FunctionData,
            .ResponseSize = &functionResponseSize
         });

   response->FunctionCode = functionCode | FUNCTION_CODE_EXCEPTION_RESPONSE_FLAG;

   *output.ResponseSize = EMPTY_PDU_SIZE + functionResponseSize;
}

bool ModbusSDeviceInternalTryProcessRequestPdu(
      void                                 *this,
      ModbusSDevicePduProcessingStageInput  input,
      ModbusSDevicePduProcessingStageOutput output)
{
   if(input.RequestSize < EMPTY_PDU_SIZE || input.RequestSize > MAX_PDU_SIZE)
      return false;

   const MessagePdu *request = input.RequestData;
   MessagePdu *response = output.ResponseData;
   uint8_t functionCode = request->FunctionCode;

   RequestFunctionProcess functionProcess;
   switch(functionCode)
   {
#if MODBUS_SDEVICE_USE_FUNCTION_04_AS_FUNCTION_03_ALIAS
      case FUNCTION_CODE_READ_INPUT_REGISTERS:
         /* fall-through */
#endif
      case FUNCTION_CODE_READ_HOLDING_REGISTERS:
         functionProcess = Process03FunctionRequest;
         break;

      case FUNCTION_CODE_PRESET_MULTIPLE_REGISTERS:
         functionProcess = Process16FunctionRequest;
         break;

      default:
         if(input.IsOutputMandatory)
         {
            EncodeExceptionResponsePdu(
                  MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_FUNCTION,
                  functionCode,
                  output);
         }
         return true;
   }

   size_t functionResponseSize;
   ModbusSDeviceProtocolException exception =
         functionProcess(
               this,
               (ModbusSDevicePduProcessingStageInput)
               {
                  .RequestData       = request->FunctionData,
                  .CallParameters    = input.CallParameters,
                  .RequestSize       = input.RequestSize - EMPTY_PDU_SIZE,
                  .IsOutputMandatory = input.IsOutputMandatory
               },
               (ModbusSDevicePduProcessingStageOutput)
               {
                  .ResponseData = response->FunctionData,
                  .ResponseSize = &functionResponseSize
               });

   if(exception == MODBUS_SDEVICE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR)
      return false;

   if(input.IsOutputMandatory)
   {
      if(exception != MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK)
      {
         EncodeExceptionResponsePdu(exception, functionCode, output);
      }
      else
      {
         response->FunctionCode = functionCode;

         *output.ResponseSize = EMPTY_PDU_SIZE + functionResponseSize;
      }
   }

   return true;
}
