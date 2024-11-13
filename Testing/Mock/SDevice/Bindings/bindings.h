#pragma once

#include "ModbusSDevice/public_base.h"

#define MOCK_REGISTERS_COUNT 5U

extern uint16_t MockReadRegisters[MOCK_REGISTERS_COUNT];
extern uint16_t MockWriteRegisters[MOCK_REGISTERS_COUNT];

ModbusSDeviceBaseProtocolException ReadRegistersMock(void                                           *handle,
                                                     const ModbusSDeviceBaseReadOperationParameters *parameters,
                                                     const void                                     *callContext);
ModbusSDeviceBaseProtocolException WriteRegistersMock(void                                            *handle,
                                                      const ModbusSDeviceBaseWriteOperationParameters *parameters,
                                                      const void                                      *callContext);
