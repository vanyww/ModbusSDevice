#pragma once

#include "SDeviceCore/config.h"

#if !defined(MODBUS_SDEVICE_USE_EXTERNAL_CONFIG) || defined(DOXYGEN)
   #define MODBUS_SDEVICE_USE_EXTERNAL_CONFIG false
#endif

#if MODBUS_SDEVICE_USE_EXTERNAL_CONFIG
   #include "modbus_sdevice_config.h"
#endif
