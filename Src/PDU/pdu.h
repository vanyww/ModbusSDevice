#pragma once

#include "../../Inc/ModbusSDevice/common.h"

#define __MODBUS_MAX_PDU_SIZE 253U

typedef struct
{
   const void *RequestContext;
} ModbusProcessingParameters;

bool TryProcessModbusPdu(SDeviceCommonHandle *, ModbusProcessingParameters, ModbusRequest *, ModbusResponse *);
