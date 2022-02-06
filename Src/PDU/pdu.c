#include "pdu.h"
#include "Functions/functions.h"

#define __MODBUS_EXCEPTION_FLAG ((uint8_t)0x80)

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

/* function codes that are implemented */
typedef enum
{
   FUNCTION_CODE_READ_HOLDING_REGISTERS    = 0x03,
   FUNCTION_CODE_PRESET_MULTIPLE_REGISTERS = 0x10
} FunctionCode;

static bool TryEncodeModbusExceptionPdu(SDeviceCommonHandle *handle,
										uint8_t functionCode,
                                        ModbusStatus exceptionCode,
                                        ModbusResponse *response)
{
   ExceptionPdu *responsePdu = response->Bytes;

   responsePdu->FunctionCode = functionCode | __MODBUS_EXCEPTION_FLAG;
   responsePdu->ExceptionCode = exceptionCode;

   response->Size = sizeof(ExceptionPdu);
   return true;
}

bool TryProcessModbusPdu(SDeviceCommonHandle *handle,
                         ModbusProcessingParameters parameters,
                         ModbusRequest *request,
                         ModbusResponse *response)
{
   if(request->Size < sizeof(CommonPdu))
      return false;

   const CommonPdu *requestPdu = request->Bytes;
   CommonPdu *responsePdu = response->Bytes;

   ModbusFunction function;
   uint8_t functionCode = requestPdu->FunctionCode;
   switch(functionCode)
   {
      case FUNCTION_CODE_READ_HOLDING_REGISTERS:
         function = ProcessModbus03FunctionRequest;
         break;

      case FUNCTION_CODE_PRESET_MULTIPLE_REGISTERS:
         function = ProcessModbus16FunctionRequest;
         break;

      default:
         SDeviceRuntimeErrorRaised(handle, MODBUS_RUNTIME_ERROR_UNIMPLEMENTED_FUNCTION_CODE);
         return TryEncodeModbusExceptionPdu(handle, functionCode, MODBUS_STATUS_ILLEGAL_FUNCTION_ERROR, response);
   }

   ModbusRequest requestFunctionData = { requestPdu->FunctionData, request->Size - sizeof(CommonPdu) };
   ModbusResponse responseFunctionData = { responsePdu->FunctionData };
   ModbusStatus status = function(handle, parameters, &requestFunctionData, &responseFunctionData);

   if(status != MODBUS_STATUS_OK)
   {
      if(status == MODBUS_STATUS_NON_MODBUS_ERROR)
         return false;

      return TryEncodeModbusExceptionPdu(handle, functionCode, status, response);
   }

   responsePdu->FunctionCode = functionCode;

   response->Size = responseFunctionData.Size + sizeof(CommonPdu);
   return true;
}
