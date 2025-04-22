#pragma once

#include "ModbusSDevice/Udp/public.h"

extern const uint8_t ModbusUdpBtuAddress[];

SDEVICE_HANDLE(ModbusUdp) * ModbusUdpMockCreateInstance(void);
