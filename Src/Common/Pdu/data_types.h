#pragma once

#include "ModbusSDevice/public_common.h"

#define MAX_PDU_SIZE 253U

typedef struct
{
   const void *RequestContext;
   ModbusSDeviceRegistersCallbacks *RegistersCallbacks;
} ModbusProcessingParameters;

typedef ModbusSDeviceProtocolException (* ModbusFunction)(void *handle,
                                                          const ModbusProcessingParameters *parameters,
                                                          const ModbusSDeviceRequest *request,
                                                          ModbusSDeviceResponse *response);
