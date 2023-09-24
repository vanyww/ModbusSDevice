#pragma once

#include "TableCrcSDevice/log_base.h"

typedef enum
{
   MODBUS_UDP_SDEVICE_STATUS_OK                    = MODBUS_SDEVICE_STATUS_OK,
   MODBUS_UDP_SDEVICE_STATUS_CORRUPTED_REQUEST     = MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST,
   MODBUS_UDP_SDEVICE_STATUS_WRONG_FUNCTION_CODE   = MODBUS_SDEVICE_STATUS_WRONG_FUNCTION_CODE,
   MODBUS_UDP_SDEVICE_STATUS_REGISTERS_ACCESS_FAIL = MODBUS_SDEVICE_STATUS_REGISTERS_ACCESS_FAIL,

   MODBUS_UDP_SDEVICE_STATUS_WRONG_PROTOCOL_ID
} ModbusUdpSDeviceStatus;
