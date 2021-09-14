#pragma once

#include "../pdu.h"

ModbusSDeviceStatus ModbusProcess16FunctionRequest(__SDEVICE_HANDLE(Modbus) *,
                                                   ModbusFunctionProcessingData *,
                                                   size_t *);
