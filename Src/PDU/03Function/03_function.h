#pragma once

#include "../pdu.h"

ModbusSDeviceStatus ModbusProcess03FunctionRequest(__SDEVICE_HANDLE(Modbus) *,
                                                   ModbusFunctionProcessingData *,
                                                   size_t *);
