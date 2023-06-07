#pragma once

#include "ModbusSDevice/public_base.h"

#define MOCK_REGISTERS_COUNT 5U

extern uint16_t MockReadRegisters[MOCK_REGISTERS_COUNT];
extern uint16_t MockWriteRegisters[MOCK_REGISTERS_COUNT];

ModbusSDeviceProtocolException ReadRegistersMock(SDEVICE_HANDLE(Modbus) *handle,
                                                 const ModbusSDeviceReadOperationParameters *parameters,
                                                 const void *callContext);
ModbusSDeviceProtocolException WriteRegistersMock(SDEVICE_HANDLE(Modbus) *handle,
                                                  const ModbusSDeviceWriteOperationParameters *parameters,
                                                  const void *callContext);
