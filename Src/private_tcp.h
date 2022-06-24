#pragma once

#include "../Inc/ModbusSDevice/core_tcp.h"

struct __SDEVICE_RUNTIME_DATA(ModbusTcp)
{
   uint8_t MbapHeaderBuffer[__MODBUS_TCP_MBAP_HEADER_SIZE];
};
