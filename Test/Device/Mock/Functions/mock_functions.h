#pragma once

#include "ModbusSDevice/common.h"

#define __MOCK_REGISTERS_COUNT 5U

extern ModbusSDeviceRegister MockReadRegisters[__MOCK_REGISTERS_COUNT];
extern ModbusSDeviceRegister MockWriteRegisters[__MOCK_REGISTERS_COUNT];

ModbusSDeviceStatus ReadRegistersMock(SDeviceCommonHandle *,
                                      ModbusSDeviceRegister *,
                                      const ModbusSDeviceOperationParameters *);

ModbusSDeviceStatus WriteRegistersMock(SDeviceCommonHandle *,
                                       const ModbusSDeviceRegister *,
                                       const ModbusSDeviceOperationParameters *);
