#include "pdu.h"
#include "Functions/03Function/03_function.h"
#include "Functions/16Function/16_function.h"

#define __MODBUS_EXCEPTION_FLAG ((uint8_t)0x80)

typedef struct
{
   uint8_t FunctionCode;
   uint8_t ExceptionCode;
} ModbusExceptionPdu;

typedef struct
{
   uint8_t FunctionCode;
   uint8_t FunctionSpecificData[];
} ModbusCommonPdu;

/* function codes that are implemented */
typedef enum
{
   FUNCTION_CODE_READ_HOLDING_REGISTERS    = 0x03,
   FUNCTION_CODE_PRESET_MULTIPLE_REGISTERS = 0x10
} FunctionCode;

bool ModbusProcessPdu(SDeviceCommonHandle *handle,
                      ModbusProcessingParameters parameters,
                      ModbusSDeviceRequest *request,
                      ModbusSDeviceResponse *response)
{
   if(request->BytesCount < sizeof(ModbusCommonPdu))
      return false;

   const ModbusCommonPdu *requestPdu = request->Bytes;
   ModbusCommonPdu *responsePdu = response->Bytes;

   ModbusSDeviceStatus (* function)(SDeviceCommonHandle *,
                                    ModbusProcessingParameters,
                                    ModbusSDeviceRequest *,
                                    ModbusSDeviceResponse *);

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
         return ModbusEncodeExceptionPdu(handle, MODBUS_SDEVICE_STATUS_ILLEGAL_FUNCTION_ERROR, request, response);
   }

   ModbusSDeviceRequest requestFunctionSpecificData =
            { requestPdu->FunctionSpecificData, request->BytesCount - sizeof(ModbusCommonPdu) };
   ModbusSDeviceResponse responseFunctionSpecificData = { responsePdu->FunctionSpecificData };

   ModbusSDeviceStatus status =
            function(handle, parameters, &requestFunctionSpecificData, &responseFunctionSpecificData);

   if(status != MODBUS_SDEVICE_STATUS_OK)
   {
      if(status == MODBUS_SDEVICE_STATUS_NON_MODBUS_ERROR)
         return false;

      return ModbusEncodeExceptionPdu(handle, status, request, response);
   }

   responsePdu->FunctionCode = requestPdu->FunctionCode;

   response->BytesCount = responseFunctionSpecificData.BytesCount + sizeof(ModbusCommonPdu);
   return true;
}

bool ModbusEncodeExceptionPdu(SDeviceCommonHandle *handle,
                              ModbusSDeviceStatus exceptionCode,
                              ModbusSDeviceRequest *request,
                              ModbusSDeviceResponse *response)
{
   const ModbusCommonPdu *requestPdu = request->Bytes;
   ModbusExceptionPdu *responsePdu = response->Bytes;

   responsePdu->FunctionCode = requestPdu->FunctionCode | __MODBUS_EXCEPTION_FLAG;
   responsePdu->ExceptionCode = exceptionCode;

   response->BytesCount = sizeof(ModbusExceptionPdu);
   return true;
}
