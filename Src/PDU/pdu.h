#pragma once

#include "../../Inc/ModbusSDevice/common.h"

#define __MODBUS_MAX_PDU_SIZE 253U

typedef struct
{
   const void *RequestContext;
} ModbusProcessingParameters;

bool ModbusProcessPdu(SDeviceCommonHandle *,
                      ModbusProcessingParameters,
                      ModbusSDeviceRequest *,
                      ModbusSDeviceResponse *);
bool ModbusEncodeExceptionPdu(SDeviceCommonHandle *,
                              ModbusSDeviceStatus,
                              ModbusSDeviceRequest *,
                              ModbusSDeviceResponse *);
