#pragma once

#include <stdint.h>
#include <stddef.h>

typedef uint16_t ModbusRtuCrcType;

ModbusRtuCrcType ModbusRtuCrcCompute(const void *, size_t);
