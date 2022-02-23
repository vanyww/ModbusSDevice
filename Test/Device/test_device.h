#pragma once

#include "ModbusSDevice/core_rtu.h"
#include "ModbusSDevice/core_tcp.h"

__SDEVICE_HANDLE(ModbusRtu) CreateModbusRtuSDevice(void);
__SDEVICE_HANDLE(ModbusTcp) CreateModbusTcpSDevice(void);
