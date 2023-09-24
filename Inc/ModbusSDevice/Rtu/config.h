#pragma once

#include "TableCrcSDevice/config_base.h"

#if !defined(MODBUS_RTU_SDEVICE_STATIC_INTERNAL_HANDLE_CONTEXT) || defined(DOXYGEN)
   #define MODBUS_RTU_SDEVICE_USE_INTERNAL_CRC true
#endif

#if !defined(MODBUS_RTU_SDEVICE_INTERNAL_CRC16_HANDLE_IDENTIFIER) || defined(DOXYGEN)
   #define MODBUS_RTU_SDEVICE_INTERNAL_CRC16_HANDLE_IDENTIFIER ((SDeviceHandleIdentifier)0xFFFF)
#endif

#if !defined(MODBUS_RTU_SDEVICE_USE_PTP_ADDRESS) || defined(DOXYGEN)
   #define MODBUS_RTU_SDEVICE_USE_PTP_ADDRESS true
#endif

#if !defined(MODBUS_RTU_SDEVICE_PTP_ADDRESS) || defined(DOXYGEN)
   #define MODBUS_RTU_SDEVICE_PTP_ADDRESS ((uint8_t)250)
#endif
