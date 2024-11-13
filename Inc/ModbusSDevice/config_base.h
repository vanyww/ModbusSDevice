#pragma once

#include "SDeviceCore/config.h"

#if !defined(MODBUS_SDEVICE_USE_EXTERNAL_CONFIG) || defined(DOXYGEN)
   #define MODBUS_SDEVICE_USE_EXTERNAL_CONFIG false
#endif

#if MODBUS_SDEVICE_USE_EXTERNAL_CONFIG
   #include "modbus_sdevice_config.h"
#endif

#if !defined(MODBUS_SDEVICE_USE_FUNCTION_04_AS_FUNCTION_03_ALIAS) || defined(DOXYGEN)
   #define MODBUS_SDEVICE_USE_FUNCTION_04_AS_FUNCTION_03_ALIAS false
#endif
