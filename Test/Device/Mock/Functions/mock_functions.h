#pragma once

#include "ModbusSDevice/core.h"

#define __MOCK_REGISTERS_COUNT 5U

extern ModbusSDeviceRegister MockReadRegisters[__MOCK_REGISTERS_COUNT];
extern ModbusSDeviceRegister MockWriteRegisters[__MOCK_REGISTERS_COUNT];

ModbusSDeviceStatus ReadRegistersMock(__SDEVICE_HANDLE(Modbus) *,
                                      ModbusSDeviceRegister *,
                                      const ModbusSDeviceOperationParameters *);

ModbusSDeviceStatus WriteRegistersMock(__SDEVICE_HANDLE(Modbus) *,
                                       const ModbusSDeviceRegister *,
                                       const ModbusSDeviceOperationParameters *);
