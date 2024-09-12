#pragma once

#include "config_base.h"
#include "dependencies_base.h"
#include "log_base.h"

#define MODBUS_SDEVICE_BASE_VERSION_MAJOR 4
#define MODBUS_SDEVICE_BASE_VERSION_MINOR 0
#define MODBUS_SDEVICE_BASE_VERSION_PATCH 0

#define MODBUS_SDEVICE_BASE_REGISTER_SIZE 2U

typedef enum
{
   MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_OK                                      = 0x00,
   MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_ILLEGAL_FUNCTION                        = 0x01,
   MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_ADDRESS                    = 0x02,
   MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_VALUE                      = 0x03,
   MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_SLAVE_DEVICE_FAILURE                    = 0x04,
   MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_ACKNOWLEDGE                             = 0x05,
   MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_SLAVE_DEVICE_BUSY                       = 0x06,
   MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_NEGATIVE_ACKNOWLEDGE                    = 0x07,
   MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_MEMORY_PARITY_ERROR                     = 0x08,
   MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_GATEWAY_PATH_UNAVAILABLE                = 0x10,
   MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND = 0x11,
   MODBUS_SDEVICE_BASE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR                      = 0xFF
} ModbusSDeviceBaseProtocolException;

typedef struct
{
   bool IsBroadcast;
} ModbusSDeviceBaseBroadcastContext;

typedef struct
{
   void    *RegistersData;
   uint16_t RegistersCount;
   uint16_t RegistersAddress;
   bool     IsRegistersDataMandatory;
} ModbusSDeviceBaseReadOperationParameters;

typedef struct
{
   const void *RegistersData;
   uint16_t    RegistersCount;
   uint16_t    RegistersAddress;
} ModbusSDeviceBaseWriteOperationParameters;

typedef struct
{
   ModbusSDeviceBaseProtocolException (* ReadOperation)(
         void                                           *handle,
         const ModbusSDeviceBaseReadOperationParameters *operationParameters,
         const void                                     *callParameters);

   ModbusSDeviceBaseProtocolException (* WriteOperation)(
         void                                            *handle,
         const ModbusSDeviceBaseWriteOperationParameters *operationParameters,
         const void                                      *callParameters);
} ModbusSDeviceBaseInitData;
