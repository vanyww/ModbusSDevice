#pragma once

#include "config.h"

#define MODBUS_SDEVICE_VERSION_MAJOR 2
#define MODBUS_SDEVICE_VERSION_MINOR 0
#define MODBUS_SDEVICE_VERSION_PATCH 0
#define MODBUS_SDEVICE_CORE_VERSION ((SDeviceVersion)                                                                  \
{                                                                                                                      \
   .Major = MODBUS_SDEVICE_VERSION_MAJOR,                                                                              \
   .Minor = MODBUS_SDEVICE_VERSION_MINOR,                                                                              \
   .Patch = MODBUS_SDEVICE_VERSION_PATCH                                                                               \
})

typedef enum
{
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK                                            = 0x00,

   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_FUNCTION_ERROR                        = 0x01,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_ADDRESS_ERROR                         = 0x02,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_ERROR                            = 0x03,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_SLAVE_DEVICE_FAILURE_ERROR                    = 0x04,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ACKNOWLEDGE                                   = 0x05,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_SLAVE_DEVICE_BUSY_ERROR                       = 0x06,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_NEGATIVE_ACKNOWLEDGE                          = 0x07,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_MEMORY_PARITY_ERROR                           = 0x08,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_GATEWAY_PATH_UNAVAILABLE_ERROR                = 0x10,
   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND_ERROR = 0x11,

   MODBUS_SDEVICE_PROTOCOL_EXCEPTION_NON_PROTOCOL_ERROR                            = 0xFF
} ModbusSDeviceProtocolException;

typedef uint16_t ModbusSDeviceRegister;

typedef struct
{
   const void *RequestContext;
   union
   {
      ModbusSDeviceRegister *AsRegisters;
      void *AsPlain;
   } Data;
   uint16_t RegistersCount;
   uint16_t RegistersAddress;
} ModbusSDeviceRegistersReadParameters;

typedef struct
{
   const void *RequestContext;
   union
   {
      const ModbusSDeviceRegister *AsRegisters;
      const void *AsPlain;
   } Data;
   uint16_t RegistersCount;
   uint16_t RegistersAddress;
} ModbusSDeviceRegistersWriteParameters;

typedef enum
{
   MODBUS_SDEVICE_STATUS_OK,
   MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST,
   MODBUS_SDEVICE_STATUS_REGISTER_ACCESS_FAIL,
   MODBUS_SDEVICE_STATUS_REGISTERS_COUNT_MISMATCH,
   MODBUS_SDEVICE_STATUS_UNIMPLEMENTED_FUNCTION_CODE,
} ModbusSDeviceStatus;

SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusRtu);
SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusTcp);

typedef union __attribute__((transparent_union))
{
   SDEVICE_HANDLE(ModbusRtu) *AsRtu;
   SDEVICE_HANDLE(ModbusTcp) *AsTcp;
   void *AsAny;
} ModbusSDeviceCommonHandle;

typedef struct
{
   ModbusSDeviceProtocolException (* ReadRegisters)(ModbusSDeviceCommonHandle handle,
                                                    const ModbusSDeviceRegistersReadParameters *parameters);
   ModbusSDeviceProtocolException (* WriteRegisters)(ModbusSDeviceCommonHandle handle,
                                                     const ModbusSDeviceRegistersWriteParameters *parameters);
} ModbusSDeviceRegistersCallbacks;

typedef struct
{
   enum
   {
      MODBUS_SDEVICE_PROTOCOL_TYPE_RTU,
      MODBUS_SDEVICE_PROTOCOL_TYPE_TCP
   } ProtocolType;
} ModbusSDeviceRequestContext;

typedef struct
{
   const void *Data;
   size_t Size;
} ModbusSDeviceRequest;

typedef struct
{
   void *const Data;
   size_t Size;
} ModbusSDeviceResponse;
