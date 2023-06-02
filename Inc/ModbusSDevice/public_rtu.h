#pragma once

#include "public_base.h"

#include <stdbool.h>

#define MODBUS_RTU_SDEVICE_INTERCHAR_DELAY_CHARS  1.5f
#define MODBUS_RTU_SDEVICE_INTERFRAME_DELAY_CHARS 3.5f

#define MODBUS_RTU_SDEVICE_PREDEFINED_DELAYS_USAGE_THERESHOLD_BAUD 19200U
#define MODBUS_RTU_SDEVICE_INTERCHAR_SECONDS_DELAY_PREDEFINED      0.750e-3f
#define MODBUS_RTU_SDEVICE_INTERFRAME_SECONDS_DELAY_PREDEFINED     1.750e-3f

#define MODBUS_RTU_SDEVICE_CHARS_TO_SECONDS_DELAY(baud, chars, char_size) (((char_size) * (chars)) / (baud))

#define MODBUS_RTU_SDEVICE_GET_INTERCHAR_DELAY(baud, char_size)                                                        \
   (((baud) > MODBUS_RTU_SDEVICE_PREDEFINED_DELAYS_USAGE_THERESHOLD_BAUD) ?                                            \
         MODBUS_RTU_SDEVICE_INTERCHAR_DELAY_PREDEFINED                    :                                            \
         MODBUS_RTU_SDEVICE_CHARS_TO_SECONDS_DELAY(baud, MODBUS_RTU_SDEVICE_INTERCHAR_DELAY_CHARS, char_size))
#define MODBUS_RTU_SDEVICE_GET_INTERFRAME_DELAY(baud, char_size)                                                       \
   (((baud) > MODBUS_RTU_SDEVICE_PREDEFINED_DELAYS_USAGE_THERESHOLD_BAUD) ?                                            \
         MODBUS_RTU_SDEVICE_INTERFRAME_DELAY_PREDEFINED                   :                                            \
         MODBUS_RTU_SDEVICE_CHARS_TO_SECONDS_DELAY(baud, MODBUS_RTU_SDEVICE_INTERFRAME_DELAY_CHARS, char_size))

#define MODBUS_RTU_SDEVICE_CRC16_POLYNOMIAL       ((uint16_t)0x8005)
#define MODBUS_RTU_SDEVICE_CRC16_INITIAL_VALUE    ((uint16_t)0xFFFF)
#define MODBUS_RTU_SDEVICE_CRC16_OUTPUT_XOR_VALUE ((uint16_t)0x0000)
#define MODBUS_RTU_SDEVICE_CRC16_IS_REVERSE       true

#define MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE ((size_t)256U)

typedef struct
{
   bool IsBroadcast;
} ModbusRtuSDeviceOperationContext;

SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusRtu);
SDEVICE_INIT_DATA_FORWARD_DECLARATION(ModbusRtu);

typedef enum
{
   MODBUS_RTU_SDEVICE_STATUS_OK                        = MODBUS_SDEVICE_STATUS_OK,
   MODBUS_RTU_SDEVICE_STATUS_CORRUPTED_REQUEST         = MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST,
   MODBUS_RTU_SDEVICE_STATUS_WRONG_FUNCTION_CODE       = MODBUS_SDEVICE_STATUS_WRONG_FUNCTION_CODE,
   MODBUS_RTU_SDEVICE_STATUS_REGISTER_ACCESS_FAIL      = MODBUS_SDEVICE_STATUS_REGISTERS_ACCESS_FAIL,
   MODBUS_RTU_SDEVICE_STATUS_SLAVE_ADDRESS_SET_INVALID
} ModbusRtuSDeviceStatus;

SDEVICE_INIT_DATA_DECLARATION(ModbusRtu)
{
   SDEVICE_INIT_DATA_DECLARATION(Modbus) BaseInit;

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

typedef struct
{
   const void *RequestData;
   size_t      RequestSize;
} ModbusRtuSDeviceInput;

typedef struct
{
   void   *ResponseData;
   size_t *ResponseSize;
} ModbusRtuSDeviceOutput;

bool ModbusRtuSDeviceTryProcessRequest(SDEVICE_HANDLE(ModbusRtu) *handle,
                                       ModbusRtuSDeviceInput      input,
                                       ModbusRtuSDeviceOutput     output);
