#pragma once

#include "config.h"
#include "dependencies.h"

#include <stdint.h>

#define MODBUS_SDEVICE_VERSION_MAJOR 3
#define MODBUS_SDEVICE_VERSION_MINOR 0
#define MODBUS_SDEVICE_VERSION_PATCH 0

#define MODBUS_SDEVICE_REGISTER_SIZE 2u

typedef enum
{
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK                                      = 0x00,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_FUNCTION                        = 0x01,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_ADDRESS                    = 0x02,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_VALUE                      = 0x03,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_SLAVE_DEVICE_FAILURE                    = 0x04,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ACKNOWLEDGE                             = 0x05,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_SLAVE_DEVICE_BUSY                       = 0x06,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_NEGATIVE_ACKNOWLEDGE                    = 0x07,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_MEMORY_PARITY_ERROR                     = 0x08,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_GATEWAY_PATH_UNAVAILABLE                = 0x10,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND = 0x11,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR                      = 0xFF
} ModbusSDeviceProtocolException;

typedef struct
{
   void    *RegistersData;
   uint16_t RegistersCount;
   uint16_t RegistersAddress;
   bool     IsRegistersDataMandatory;
} ModbusSDeviceReadOperationParameters;

typedef struct
{
   const void *RegistersData;
   uint16_t    RegistersCount;
   uint16_t    RegistersAddress;
} ModbusSDeviceWriteOperationParameters;

typedef struct
{
   ModbusSDeviceProtocolException (* ReadOperation)(
         void                                       *this,
         const ModbusSDeviceReadOperationParameters *operationParameters,
         const void                                 *callParameters);

   ModbusSDeviceProtocolException (* WriteOperation)(
         void                                        *this,
         const ModbusSDeviceWriteOperationParameters *operationParameters,
         const void                                  *callParameters);
} __attribute__((may_alias)) ModbusSDeviceInitData;
