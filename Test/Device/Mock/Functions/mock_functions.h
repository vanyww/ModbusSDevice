#pragma once

#include "ModbusSDevice/common.h"

#define __MOCK_REGISTERS_COUNT 5U

extern ModbusRegister MockReadRegisters[__MOCK_REGISTERS_COUNT];
extern ModbusRegister MockWriteRegisters[__MOCK_REGISTERS_COUNT];

ModbusStatus ReadRegistersMock(SDeviceCommonHandle *, ModbusRegister *, const ModbusOperationParameters *);
ModbusStatus WriteRegistersMock(SDeviceCommonHandle *, const ModbusRegister *, const ModbusOperationParameters *);
