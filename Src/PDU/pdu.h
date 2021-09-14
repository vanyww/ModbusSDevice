#pragma once

#include "ModbusSDevice/core.h"

#define __MODBUS_MAX_PDU_SIZE 253U

typedef struct
{
   uint8_t FunctionCode;
   uint8_t FunctionSpecificData[];
} ModbusCommonPdu;

typedef struct
{
   const void *RequestParameters;
   size_t PduSize;
} ModbusCommonRequestProcessingData;

typedef struct
{
   const void *RequestParameters;
   size_t FunctionSpecificDataSize;
} ModbusFunctionProcessingData;

size_t ModbusProcessPdu(__SDEVICE_HANDLE(Modbus) *, ModbusCommonRequestProcessingData *);
size_t ModbusEncodeExceptionPdu(__SDEVICE_HANDLE(Modbus) *, ModbusSDeviceStatus);
