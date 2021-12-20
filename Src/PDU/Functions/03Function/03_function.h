#pragma once

#include "../../pdu.h"

ModbusSDeviceStatus ModbusProcess03FunctionRequest(SDeviceCommonHandle *,
                                                   ModbusProcessingParameters,
                                                   ModbusSDeviceRequest *,
                                                   ModbusSDeviceResponse *);
