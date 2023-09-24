#pragma once

#include "ModbusSDevice/dependencies_base.h"

#include "config.h"

#if MODBUS_RTU_SDEVICE_USE_INTERNAL_CRC
   #include "TableCrcSDevice/Crc16/public.h"
#endif

#if (TABLE_CRC16_SDEVICE_VERSION_MAJOR != 4) || (TABLE_CRC16_SDEVICE_VERSION_MINOR < 0)
   #error TableCrc16 SDevice version is incorrect.
#endif
