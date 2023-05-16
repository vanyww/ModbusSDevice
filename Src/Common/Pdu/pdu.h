#pragma once

#include "Functions/03Function/03_function.h"
#include "Functions/16Function/16_function.h"

#include <stdbool.h>

#define MODBUS_EXCEPTION_RESPONSE_FUNCTION_CODE_FLAG_MASK ((uint8_t)0x80)

typedef struct
{
   uint8_t FunctionCode;
   uint8_t ExceptionCode;
} ExceptionPdu;

typedef struct
{
   uint8_t FunctionCode;
   uint8_t FunctionData[];
} CommonPdu;

typedef enum
{
   FUNCTION_CODE_READ_HOLDING_REGISTERS    = 0x03,
   FUNCTION_CODE_READ_INPUT_REGISTERS      = 0x04,
   FUNCTION_CODE_PRESET_MULTIPLE_REGISTERS = 0x10
} FunctionCode;

static void EncodeExceptionResponsePdu(void *handle,
                                       ModbusSDeviceStatus exceptionCode,
                                       uint8_t functionCode,
                                       ModbusSDeviceResponse *response)
{
   SDeviceDebugAssert(response != NULL);
   SDeviceDebugAssert(handle != NULL);

   ExceptionPdu *responseData = response->Data;

   responseData->FunctionCode = functionCode | MODBUS_EXCEPTION_RESPONSE_FUNCTION_CODE_FLAG_MASK;
   responseData->ExceptionCode = exceptionCode;

   response->Size = sizeof(ExceptionPdu);
}

static bool TryProcessRequestPdu(void *handle,
                                 const ModbusProcessingParameters *parameters,
                                 const ModbusSDeviceRequest *request,
                                 ModbusSDeviceResponse *response)
{
   SDeviceDebugAssert(request != NULL);
   SDeviceDebugAssert(response != NULL);
   SDeviceDebugAssert(parameters != NULL);
   SDeviceDebugAssert(handle != NULL);

   if(request->Size < sizeof(CommonPdu))
      return false;

   const CommonPdu *requestData = request->Data;
   CommonPdu *responseData = response->Data;

   ModbusFunction function;
   uint8_t functionCode = requestData->FunctionCode;
   switch(functionCode)
   {
      case FUNCTION_CODE_READ_INPUT_REGISTERS:
         /* fall-through */
      case FUNCTION_CODE_READ_HOLDING_REGISTERS:
         function = Process03FunctionRequest;
         break;

      case FUNCTION_CODE_PRESET_MULTIPLE_REGISTERS:
         function = Process16FunctionRequest;
         break;

      default:
         SDeviceLogStatus(handle, MODBUS_SDEVICE_STATUS_UNIMPLEMENTED_FUNCTION_CODE);
         EncodeExceptionResponsePdu(handle, MODBUS_SDEVICE_STATUS_UNIMPLEMENTED_FUNCTION_CODE, functionCode, response);
         return true;
   }

   ModbusSDeviceRequest requestFunctionData = { requestData->FunctionData, request->Size - sizeof(CommonPdu) };
   ModbusSDeviceResponse responseFunctionData = { responseData->FunctionData };
   ModbusSDeviceProtocolException status = function(handle, parameters, &requestFunctionData, &responseFunctionData);

   if(status != MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK)
   {
      if(status == MODBUS_SDEVICE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR)
         return false;

      EncodeExceptionResponsePdu(handle, status, functionCode, response);

      return true;
   }

   responseData->FunctionCode = functionCode;

   response->Size = responseFunctionData.Size + sizeof(CommonPdu);

   return true;
}
