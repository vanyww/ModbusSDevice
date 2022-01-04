#pragma once

#include <stdint.h>
#include <stddef.h>

typedef uint16_t ModbusRtuCrcType;

ModbusRtuCrcType ComputeModbusRtuCrc(const void *, size_t);
