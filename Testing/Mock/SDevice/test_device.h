#pragma once

#include "ModbusSDevice/Rtu/public.h"
#include "ModbusSDevice/Tcp/public.h"
#include "ModbusSDevice/Udp/public.h"

extern const uint8_t ModbusUdpBtuAddress[];

SDEVICE_HANDLE(ModbusRtu) * CreateModbusRtuSDevice(void);
SDEVICE_HANDLE(ModbusTcp) * CreateModbusTcpSDevice(void);
SDEVICE_HANDLE(ModbusUdp) * CreateModbusUdpSDevice(void);
