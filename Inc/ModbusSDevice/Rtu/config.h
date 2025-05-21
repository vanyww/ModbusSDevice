#pragma once

#include "../config.h"

#if !defined(MODBUS_RTU_SDEVICE_USE_PTP_ADDRESS)
   #define MODBUS_RTU_SDEVICE_USE_PTP_ADDRESS true
#endif

#if !defined(MODBUS_RTU_SDEVICE_PTP_ADDRESS)
   #define MODBUS_RTU_SDEVICE_PTP_ADDRESS ((uint8_t)250)
#endif
