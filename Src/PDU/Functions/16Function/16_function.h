#pragma once

#include "../../pdu.h"

ModbusSDeviceStatus ModbusProcess16FunctionRequest(SDeviceCommonHandle *,
                                                   ModbusProcessingParameters,
                                                   ModbusSDeviceRequest *,
                                                   ModbusSDeviceResponse *);
