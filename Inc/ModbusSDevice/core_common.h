#pragma once

#include "SDeviceCore/interface.h"

typedef enum
{
   MODBUS_STATUS_OK                                            = 0x00,

   MODBUS_STATUS_ILLEGAL_FUNCTION_ERROR                        = 0x01,
   MODBUS_STATUS_ILLEGAL_ADDRESS_ERROR                         = 0x02,
   MODBUS_STATUS_ILLEGAL_DATA_ERROR                            = 0x03,
   MODBUS_STATUS_SLAVE_DEVICE_FAILURE_ERROR                    = 0x04,
   MODBUS_STATUS_ACKNOWLEDGE                                   = 0x05,
   MODBUS_STATUS_SLAVE_DEVICE_BUSY_ERROR                       = 0x06,
   MODBUS_STATUS_NEGATIVE_ACKNOWLEDGE                          = 0x07,
   MODBUS_STATUS_MEMORY_PARITY_ERROR                           = 0x08,
   MODBUS_STATUS_GATEWAY_PATH_UNAVAILABLE_ERROR                = 0x10,
   MODBUS_STATUS_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND_ERROR = 0x11,

   MODBUS_STATUS_NON_MODBUS_ERROR                              = 0xFF
} ModbusStatus;

typedef enum
{
   MODBUS_RUNTIME_ERROR_WRONG_REQUEST_SIZE          = 0x01,
   MODBUS_RUNTIME_ERROR_REGISTER_ACCESS_FAIL        = 0x02,
   MODBUS_RUNTIME_ERROR_WRONG_REGISTERS_COUNT       = 0x03,
   MODBUS_RUNTIME_ERROR_UNIMPLEMENTED_FUNCTION_CODE = 0x04,
   MODBUS_RUNTIME_ERROR_REQUEST_CRC_MISMATCH        = 0x05,
   MODBUS_RUNTIME_ERROR_SLAVE_ADDRESS_INVALID_SET   = 0x06
} ModbusRuntimeError;

typedef enum
{
   MODBUS_MODBUS_TYPE_RTU,
   MODBUS_MODBUS_TYPE_TCP
} ModbusModbusType;

typedef struct
{
   ModbusModbusType ModbusType;
} ModbusRequestData;

typedef union __attribute__((packed, scalar_storage_order("big-endian")))
{
   uint16_t AsValue;
   uint8_t AsBytes[sizeof(uint16_t)];
} ModbusRegister;

typedef struct
{
   const void *RequestContext;
   size_t RegistersCount;
   uint16_t RegisterAddress;
} ModbusOperationParameters;

typedef struct
{
   const void *Bytes;
   size_t Size;
} ModbusRequest;

typedef struct
{
   void *Bytes;
   size_t Size;
} ModbusResponse;

typedef struct
{
   ModbusStatus (* ReadRegisters)(SDeviceCommonHandle *, ModbusRegister *, const ModbusOperationParameters *);
   ModbusStatus (* WriteRegisters)(SDeviceCommonHandle *, const ModbusRegister *, const ModbusOperationParameters *);
} __SDEVICE_INIT_DATA(Modbus);

