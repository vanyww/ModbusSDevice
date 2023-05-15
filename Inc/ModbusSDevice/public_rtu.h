#pragma once

#include "public_common.h"

#include <math.h>
#include <stdbool.h>

#define MODBUS_RTU_SDEVICE_INTERCHAR_DELAY_CHARS 1.5f
#define MODBUS_RTU_SDEVICE_INTERFRAME_DELAY_CHARS 3.5f

#define MODBUS_RTU_SDEVICE_PREDEFINED_DELAYS_USAGE_THERESHOLD_BAUD 19200
#define MODBUS_RTU_SDEVICE_INTERCHAR_DELAY_PREDEFINED 0.750e-3f
#define MODBUS_RTU_SDEVICE_INTERFRAME_DELAY_PREDEFINED 1.750e-3f

#define MODBUS_RTU_SDEVICE_CHARS_TO_SECONDS_DELAY(baud, chars, char_size) (((char_size) * (chars)) / (baud))

#define MODBUS_RTU_SDEVICE_GET_INTERCHAR_DELAY(baud, char_size)                                                        \
    (((baud) > MODBUS_RTU_SDEVICE_PREDEFINED_DELAYS_USAGE_THERESHOLD_BAUD) ?                                           \
      MODBUS_RTU_SDEVICE_INTERCHAR_DELAY_PREDEFINED                        :                                           \
      MODBUS_RTU_SDEVICE_CHARS_TO_SECONDS_DELAY(baud, MODBUS_RTU_SDEVICE_INTERCHAR_DELAY_CHARS, char_size))
#define MODBUS_RTU_SDEVICE_GET_INTERFRAME_DELAY(baud, char_size)                                                       \
    (((baud) > MODBUS_RTU_SDEVICE_PREDEFINED_DELAYS_USAGE_THERESHOLD_BAUD) ?                                           \
      MODBUS_RTU_SDEVICE_INTERFRAME_DELAY_PREDEFINED                       :                                           \
      MODBUS_RTU_SDEVICE_CHARS_TO_SECONDS_DELAY(baud, MODBUS_RTU_SDEVICE_INTERFRAME_DELAY_CHARS, char_size))

#define MODBUS_RTU_SDEVICE_CRC16_POLYNOMIAL 0x8005
#define MODBUS_RTU_SDEVICE_CRC16_INITIAL_VALUE 0xFFFF
#define MODBUS_RTU_SDEVICE_CRC16_OUTPUT_XOR_VALUE 0x0000
#define MODBUS_RTU_SDEVICE_CRC16_IS_REVERSE true

#define MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE 256 /* 253 (max PDU) + 3 (ADU members) */

typedef struct
{
   ModbusSDeviceRequestContext Common;
   enum
   {
      MODBUS_RTU_SDEVICE_REQUEST_TYPE_UNICAST,
      MODBUS_RTU_SDEVICE_REQUEST_TYPE_BROADCAST
   } RequestType;
} ModbusRtuSDeviceRequestContext;

SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusRtu);
SDEVICE_INIT_DATA_FORWARD_DECLARATION(ModbusRtu);

typedef enum
{
   MODBUS_RTU_SDEVICE_STATUS_OK                          = MODBUS_SDEVICE_STATUS_OK,
   MODBUS_RTU_SDEVICE_STATUS_CORRUPTED_REQUEST           = MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST,
   MODBUS_RTU_SDEVICE_STATUS_REGISTER_ACCESS_FAIL        = MODBUS_SDEVICE_STATUS_REGISTER_ACCESS_FAIL,
   MODBUS_RTU_SDEVICE_STATUS_REGISTERS_COUNT_MISMATCH    = MODBUS_SDEVICE_STATUS_REGISTERS_COUNT_MISMATCH,
   MODBUS_RTU_SDEVICE_STATUS_UNIMPLEMENTED_FUNCTION_CODE = MODBUS_SDEVICE_STATUS_UNIMPLEMENTED_FUNCTION_CODE,

   MODBUS_RTU_SDEVICE_STATUS_SLAVE_ADDRESS_INVALID_SET
} ModbusRtuSDeviceStatus;

SDEVICE_INIT_DATA_DECLARATION(ModbusRtu)
{
   ModbusSDeviceRegistersCallbacks RegistersCallbacks;

#if !defined MODBUS_RTU_SDEVICE_USE_INTERNAL_CRC
   uint16_t (* ComputeCrc16)(SDEVICE_HANDLE(ModbusRtu) *handle, const void *data, size_t size);
#endif
};

SDEVICE_STRING_NAME_DECLARATION(ModbusRtu);

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusRtu, init, owner, identifier, context);
SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusRtu, handlePointer);

SDEVICE_PROPERTY_TYPE_DECLARATION(ModbusRtu, SlaveAddress, uint8_t);
SDEVICE_GET_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, handle, value);
SDEVICE_SET_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, handle, value);

bool ModbusRtuSDeviceTryProcessRequest(SDEVICE_HANDLE(ModbusRtu) *handle,
                                       const ModbusSDeviceRequest *request,
                                       ModbusSDeviceResponse *response);
