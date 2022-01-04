#pragma once

#include "03Function/03_function.h"
#include "16Function/16_function.h"

typedef ModbusStatus (* ModbusFunction)(SDeviceCommonHandle *,
                                        ModbusProcessingParameters,
                                        ModbusRequest *,
                                        ModbusResponse *);
