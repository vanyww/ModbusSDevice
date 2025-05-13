#pragma once

#include "TableCrcSDevice/config.h"

#if !defined(MODBUS_RTU_SDEVICE_STATIC_INTERNAL_HANDLE_CONTEXT)
   #define MODBUS_RTU_SDEVICE_USE_INTERNAL_CRC true
#endif

#if !defined(MODBUS_RTU_SDEVICE_USE_PTP_ADDRESS)
   #define MODBUS_RTU_SDEVICE_USE_PTP_ADDRESS true
#endif

#if !defined(MODBUS_RTU_SDEVICE_PTP_ADDRESS)
   #define MODBUS_RTU_SDEVICE_PTP_ADDRESS ((uint8_t)250)
#endif
