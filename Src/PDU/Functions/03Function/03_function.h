#pragma once

#include "../../pdu.h"

ModbusStatus ProcessModbus03FunctionRequest(SDeviceCommonHandle *,
                                            ModbusProcessingParameters,
                                            ModbusRequest *,
                                            ModbusResponse *);
