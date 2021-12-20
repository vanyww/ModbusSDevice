#pragma once

#include "ModbusSDevice/core_rtu.h"
#include "ModbusSDevice/core_tcp.h"

void CreateModbusRtuSDevice(__SDEVICE_HANDLE(ModbusRtu) *);
void CreateModbusTcpSDevice(__SDEVICE_HANDLE(ModbusTcp) *);
