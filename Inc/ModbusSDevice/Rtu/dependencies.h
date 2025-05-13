#pragma once

#include "ModbusSDevice/dependencies.h"

#if MODBUS_RTU_SDEVICE_USE_INTERNAL_CRC
   #include "TableCrcSDevice/Crc16/public.h"

   #if (TABLE_CRC16_SDEVICE_VERSION_MAJOR != 5) || (TABLE_CRC16_SDEVICE_VERSION_MINOR < 0)
      #error TableCrc16 SDevice version is incorrect.
   #endif
#endif
