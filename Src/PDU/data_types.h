#pragma once

#include "../private_rtu.h"
#include "../private_tcp.h"

#define MAX_PDU_SIZE 253U

typedef struct
{
   const void *RequestContext;
   ModbusSDeviceRegistersCallbacks *RegistersCallbacks;
} ModbusProcessingParameters;

typedef ModbusSDeviceProtocolException (* ModbusFunction)(ModbusSDeviceCommonHandle handle,
                                                          const ModbusProcessingParameters *parameters,
                                                          const ModbusSDeviceRequest *request,
                                                          ModbusSDeviceResponse *response);
