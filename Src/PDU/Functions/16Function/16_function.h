#pragma once

#include "../../pdu.h"

ModbusStatus ProcessModbus16FunctionRequest(SDeviceCommonHandle *,
                                            ModbusProcessingParameters,
                                            ModbusRequest *,
                                            ModbusResponse *);
