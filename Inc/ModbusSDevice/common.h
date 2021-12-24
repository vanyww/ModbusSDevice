#pragma once

#include "SDeviceCore/interface.h"

#include <stddef.h>

typedef enum
{
   MODBUS_SDEVICE_STATUS_OK                                            = 0x00,

   MODBUS_SDEVICE_STATUS_ILLEGAL_FUNCTION_ERROR                        = 0x01,
   MODBUS_SDEVICE_STATUS_ILLEGAL_ADDRESS_ERROR                         = 0x02,
   MODBUS_SDEVICE_STATUS_ILLEGAL_DATA_ERROR                            = 0x03,
   MODBUS_SDEVICE_STATUS_SLAVE_DEVICE_FAILURE_ERROR                    = 0x04,
   MODBUS_SDEVICE_STATUS_ACKNOWLEDGE                                   = 0x05,
   MODBUS_SDEVICE_STATUS_SLAVE_DEVICE_BUSY_ERROR                       = 0x06,
   MODBUS_SDEVICE_STATUS_NEGATIVE_ACKNOWLEDGE                          = 0x07,
   MODBUS_SDEVICE_STATUS_MEMORY_PARITY_ERROR                           = 0x08,
   MODBUS_SDEVICE_STATUS_GATEWAY_PATH_UNAVAILABLE_ERROR                = 0x10,
   MODBUS_SDEVICE_STATUS_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND_ERROR = 0x11,

   MODBUS_SDEVICE_STATUS_NON_MODBUS_ERROR                              = 0xFF
} ModbusSDeviceStatus;

typedef enum
{
   MODBUS_SDEVICE_RUNTIME_REQUEST_SIZE_ERROR                     = 0x01,
   MODBUS_SDEVICE_RUNTIME_REGISTER_ACCESS_ERROR                  = 0x02,
   MODBUS_SDEVICE_RUNTIME_REQUEST_REGISTER_COUNT_ERROR           = 0x03,
   MODBUS_SDEVICE_RUNTIME_REQUEST_FUNCTION_CODE_ERROR            = 0x04,
   MODBUS_SDEVICE_RUNTIME_REQUEST_CRC_ERROR                      = 0x05,
   MODBUS_SDEVICE_RUNTIME_SLAVE_ADDRESS_SETTING_VALIDATION_ERROR = 0x06
} ModbusSDeviceRuntimeError;

typedef enum
{
   MODBUS_SDEVICE_MODBUS_TYPE_RTU,
   MODBUS_SDEVICE_MODBUS_TYPE_TCP
} ModbusSDeviceModbusType;

typedef struct
{
   ModbusSDeviceModbusType ModbusType;
} ModbusSDeviceRequestData;

typedef union __attribute__((packed, scalar_storage_order("big-endian")))
{
   uint16_t AsValue;
   uint8_t AsBytes[sizeof(uint16_t)];
} ModbusSDeviceRegister;

typedef struct
{
   const void *RequestContext;
   size_t RegistersCount;
   uint16_t RegisterAddress;
} ModbusSDeviceOperationParameters;

typedef struct
{
   const void *Bytes;
   size_t BytesCount;
} ModbusSDeviceRequest;

typedef struct __attribute__((scalar_storage_order("big-endian")))
{
   void *Bytes;
   size_t BytesCount;
} ModbusSDeviceResponse;

typedef struct
{
   ModbusSDeviceStatus (*ReadRegistersFunction)(SDeviceCommonHandle *,
                                                ModbusSDeviceRegister *,
                                                const ModbusSDeviceOperationParameters *);
   ModbusSDeviceStatus (*WriteRegistersFunction)(SDeviceCommonHandle *,
                                                 const ModbusSDeviceRegister *,
                                                 const ModbusSDeviceOperationParameters *);
} __SDEVICE_CONSTANT_DATA(Modbus);

