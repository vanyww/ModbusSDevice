#pragma once

#include "SDeviceCore/interface.h"

#include <stdint.h>
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
   MODBUS_SDEVICE_TYPE_RTU,
   MODBUS_SDEVICE_TYPE_TCP
} ModbusSDeviceType;

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

/* Satty's interface start */

__SDEVICE_HANDLE_FORWARD_DECLARATION(Modbus);

typedef struct
{
   ModbusSDeviceStatus (*ReadRegistersFunction)(__SDEVICE_HANDLE(Modbus) *,
                                                ModbusSDeviceRegister *,
                                                const ModbusSDeviceOperationParameters *);
   ModbusSDeviceStatus (*WriteRegistersFunction)(__SDEVICE_HANDLE(Modbus) *,
                                                 const ModbusSDeviceRegister *,
                                                 const ModbusSDeviceOperationParameters *);
   void *TransmitBuffer;
   void *ReceiveBuffer;
   ModbusSDeviceType Type;
} __SDEVICE_CONSTANT_DATA(Modbus);

typedef struct { } ModbusSDeviceTcpSettings;

typedef struct
{
   uint8_t SlaveAddress;
} ModbusSDeviceRtuSettings;

typedef union
{
   ModbusSDeviceRtuSettings Rtu;
   ModbusSDeviceTcpSettings Tcp;
} __SDEVICE_SETTINGS_DATA(Modbus);

typedef struct
{
   void *ReceiveBufferPdu;
   void *ReceiveBufferFunctionSpecificData;
   void *TransmitBufferPdu;
   void *TransmitBufferFunctionSpecificData;
} __SDEVICE_DYNAMIC_DATA(Modbus);

__SDEVICE_HANDLE_DEFINITION(Modbus);

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(Modbus,);

typedef enum
{
   MODBUS_SDEVICE_RUNTIME_REQUEST_SIZE_ERROR                     = 0x01,
   MODBUS_SDEVICE_RUNTIME_REGISTER_ACCESS_ERROR                  = 0x02,
   MODBUS_SDEVICE_RUNTIME_REQUEST_REGISTER_COUNT_ERROR           = 0x03,
   MODBUS_SDEVICE_RUNTIME_REQUEST_FUNCTION_CODE_ERROR            = 0x04,
   MODBUS_SDEVICE_RUNTIME_REQUEST_CRC_ERROR                      = 0x05,
   MODBUS_SDEVICE_RUNTIME_SLAVE_ADDRESS_SETTING_VALIDATION_ERROR = 0x06
} ModbusSDeviceRuntimeError;

/* Satty's interface end */

size_t ModbusSDeviceProcessRequest(__SDEVICE_HANDLE(Modbus) *, size_t);
