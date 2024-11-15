#pragma once

#include "TableCrcSDevice/log_base.h"

typedef enum
{
   MODBUS_UDP_SDEVICE_STATUS_OK                    = MODBUS_SDEVICE_STATUS_OK,
   MODBUS_UDP_SDEVICE_STATUS_WRONG_REQUEST_SIZE    = MODBUS_SDEVICE_STATUS_WRONG_REQUEST_SIZE,
   MODBUS_UDP_SDEVICE_STATUS_WRONG_FUNCTION_CODE   = MODBUS_SDEVICE_STATUS_WRONG_FUNCTION_CODE,
   MODBUS_UDP_SDEVICE_STATUS_WRONG_REGISTERS_COUNT = MODBUS_SDEVICE_STATUS_WRONG_REGISTERS_COUNT,

   MODBUS_UDP_SDEVICE_STATUS_WRONG_PROTOCOL_ID
} ModbusUdpSDeviceStatus;
