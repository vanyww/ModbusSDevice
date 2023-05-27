#pragma once

#include "ModbusSDevice/public_rtu.h"
#include "ModbusSDevice/public_tcp.h"
#include "ModbusSDevice/public_udp.h"

extern const uint8_t ModbusUdpBtuAddress[];

SDEVICE_HANDLE(ModbusRtu) * CreateModbusRtuSDevice(void);
SDEVICE_HANDLE(ModbusTcp) * CreateModbusTcpSDevice(void);
SDEVICE_HANDLE(ModbusUdp) * CreateModbusUdpSDevice(void);
