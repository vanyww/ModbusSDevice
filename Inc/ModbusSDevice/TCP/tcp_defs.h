#pragma once

#include "SDeviceCore/interface.h"

#include <stdint.h>

#define __MODBUS_SDEVICE_TCP_MAX_MESSAGE_SIZE 260U /* 253 (max PDU) + 7 (common ADU members) */

__SDEVICE_HANDLE_FORWARD_DECLARATION(Modbus);

typedef struct {
    uint8_t SlaveAddress;
} ModbusSDeviceTcpRequestData;
