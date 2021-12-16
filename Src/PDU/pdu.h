#pragma once

#include "../../Inc/ModbusSDevice/core.h"

#define __MODBUS_MAX_PDU_SIZE 253U

typedef struct
{
   const void *RequestContext;
} ModbusProcessingParameters;

bool ModbusProcessPdu(__SDEVICE_HANDLE(Modbus) *,
                      ModbusProcessingParameters,
                      ModbusSDeviceRequest *,
                      ModbusSDeviceResponse *);
bool ModbusEncodeExceptionPdu(__SDEVICE_HANDLE(Modbus) *,
                              ModbusSDeviceStatus,
                              ModbusSDeviceRequest *,
                              ModbusSDeviceResponse *);
