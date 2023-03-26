#pragma once

#include "ModbusSDevice/public_rtu.h"
#include "ModbusSDevice/public_tcp.h"

SDEVICE_HANDLE(ModbusRtu) * CreateModbusRtuSDevice(void);
SDEVICE_HANDLE(ModbusTcp) * CreateModbusTcpSDevice(void);
