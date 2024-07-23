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

typedef ThisBaseProtocolException (* RequestFunctionProcessFunction)(
      void                    *handle,
      PduProcessingStageInput  input,
      PduProcessingStageOutput output);

static void EncodeExceptionResponsePdu(
      ThisBaseProtocolException exception,
      FunctionCode              functionCode,
      PduProcessingStageOutput  output)
{
   MessagePdu *response = output.ResponseData;

   size_t functionResponseSize;
   ProcessExceptionFunction(
         exception,
         (PduProcessingStageOutput)
         {
            .ResponseData = response->FunctionData,
            .ResponseSize = &functionResponseSize
         });

   response->FunctionCode = functionCode | FUNCTION_CODE_EXCEPTION_RESPONSE_FLAG;

   *output.ResponseSize = EMPTY_PDU_SIZE + functionResponseSize;
}

bool ModbusSDeviceBaseTryProcessRequestPdu(
      void                    *handle,
      PduProcessingStageInput  input,
      PduProcessingStageOutput output)
{
   if(input.RequestSize < EMPTY_PDU_SIZE || input.RequestSize > MAX_PDU_SIZE)
   {
      SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_WRONG_REQUEST_SIZE);
      return false;
   }

   const MessagePdu *request  = input.RequestData;
   MessagePdu       *response = output.ResponseData;

   uint8_t functionCode = request->FunctionCode;

   RequestFunctionProcessFunction functionProcessor;
   switch(functionCode)
   {
#if MODBUS_SDEVICE_USE_FUNCTION_04_AS_FUNCTION_03_ALIAS
      case FUNCTION_CODE_READ_INPUT_REGISTERS:
         /* fall-through */
#endif
      case FUNCTION_CODE_READ_HOLDING_REGISTERS:
         functionProcessor = Process03FunctionRequest;
         break;

      case FUNCTION_CODE_PRESET_MULTIPLE_REGISTERS:
         functionProcessor = Process16FunctionRequest;
         break;

      default:
         if(!SupportsBroadcasting(handle) || !((const ThisBaseBroadcastContext *)input.OperationContext)->IsBroadcast)
            SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_WRONG_FUNCTION_CODE);

         if(input.IsOutputMandatory)
            EncodeExceptionResponsePdu(MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_ILLEGAL_FUNCTION, functionCode, output);

         return true;
   }

   size_t functionResponseSize;
   ThisBaseProtocolException exception =
         functionProcessor(
               handle,
               (PduProcessingStageInput)
               {
                  .RequestData       = request->FunctionData,
                  .OperationContext  = input.OperationContext,
                  .RequestSize       = input.RequestSize - EMPTY_PDU_SIZE,
                  .IsOutputMandatory = input.IsOutputMandatory
               },
               (PduProcessingStageOutput)
               {
                  .ResponseData = response->FunctionData,
                  .ResponseSize = &functionResponseSize
               });

   if(exception == MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR)
      return false;

   if(input.IsOutputMandatory)
   {
      if(exception != MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_OK)
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
