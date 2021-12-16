#pragma once

#include "../../pdu.h"

ModbusSDeviceStatus ModbusProcess16FunctionRequest(__SDEVICE_HANDLE(Modbus) *,
                                                   ModbusProcessingParameters,
                                                   ModbusSDeviceRequest *,
                                                   ModbusSDeviceResponse *);
