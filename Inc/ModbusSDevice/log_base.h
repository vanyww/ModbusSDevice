#pragma once

#include "SDeviceCore/errors.h"

typedef enum
{
   MODBUS_SDEVICE_STATUS_OK,
   MODBUS_SDEVICE_STATUS_WRONG_REQUEST_SIZE,
   MODBUS_SDEVICE_STATUS_WRONG_FUNCTION_CODE,
   MODBUS_SDEVICE_STATUS_WRONG_REGISTERS_COUNT
} ModbusSDeviceStatus;
