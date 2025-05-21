#pragma once

#include "ModbusSDevice/public.h"

#define MOCK_REGISTERS_COUNT 5u

extern uint16_t MockReadRegisters[MOCK_REGISTERS_COUNT];
extern uint16_t MockWriteRegisters[MOCK_REGISTERS_COUNT];

ModbusSDeviceProtocolException MockReadOperation(void                                       *handle,
                                                 const ModbusSDeviceReadOperationParameters *parameters,
                                                 const void                                 *callContext);

ModbusSDeviceProtocolException MockWriteOperation(void                                        *handle,
                                                  const ModbusSDeviceWriteOperationParameters *parameters,
                                                  const void                                  *callContext);
