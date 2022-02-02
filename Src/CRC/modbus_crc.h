#pragma once

#include "../../Inc/ModbusSDevice/core_rtu.h"

ModbusRtuCrcType ComputeModbusRtuCrc(__SDEVICE_HANDLE(ModbusRtu) *, const void *, size_t);
