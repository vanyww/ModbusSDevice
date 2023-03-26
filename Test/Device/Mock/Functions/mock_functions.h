#pragma once

#include "ModbusSDevice/public_common.h"

#define MOCK_REGISTERS_COUNT 5U

extern ModbusSDeviceRegister MockReadRegisters[MOCK_REGISTERS_COUNT];
extern ModbusSDeviceRegister MockWriteRegisters[MOCK_REGISTERS_COUNT];

ModbusSDeviceProtocolException ReadRegistersMock(ModbusSDeviceCommonHandle handle,
                                                 const ModbusSDeviceRegistersReadParameters *parameters);
ModbusSDeviceProtocolException WriteRegistersMock(ModbusSDeviceCommonHandle handle,
                                                  const ModbusSDeviceRegistersWriteParameters *parameters);
