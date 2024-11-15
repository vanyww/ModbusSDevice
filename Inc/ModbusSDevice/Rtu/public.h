#pragma once

#include "ModbusSDevice/public_base.h"

#include "config.h"
#include "dependencies.h"
#include "log.h"

/* 1A9D48ED-6FA8-11EF-A767-AFDC253B5835 */
#define MODBUS_RTU_SDEVICE_UUID_HIGH 0x1A9D48ED6FA811EF
#define MODBUS_RTU_SDEVICE_UUID_LOW  0xA767AFDC253B5835

#define MODBUS_RTU_SDEVICE_VERSION_MAJOR MODBUS_SDEVICE_BASE_VERSION_MAJOR
#define MODBUS_RTU_SDEVICE_VERSION_MINOR MODBUS_SDEVICE_BASE_VERSION_MINOR
#define MODBUS_RTU_SDEVICE_VERSION_PATCH MODBUS_SDEVICE_BASE_VERSION_PATCH

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

#define MODBUS_RTU_SDEVICE_CRC16_POLYNOMIAL       0x8005
#define MODBUS_RTU_SDEVICE_CRC16_INITIAL_VALUE    0xFFFF
#define MODBUS_RTU_SDEVICE_CRC16_OUTPUT_XOR_VALUE 0x0000
#define MODBUS_RTU_SDEVICE_CRC16_IS_REVERSE       true

#define MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE 256U

typedef struct
{
   ModbusSDeviceBaseBroadcastContext Base;
} ModbusRtuSDeviceCallParameters;

SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusRtu);
SDEVICE_INIT_DATA_FORWARD_DECLARATION(ModbusRtu);

SDEVICE_INIT_DATA_DECLARATION(ModbusRtu)
{
   ModbusSDeviceBaseInitData Base;

#if !MODBUS_RTU_SDEVICE_USE_INTERNAL_CRC
   uint16_t (* ComputeCrc16)(SDEVICE_HANDLE(ModbusRtu) *handle, const void *data, size_t size);
#endif
};

SDEVICE_IDENTITY_BLOCK_DECLARATION(ModbusRtu);

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusRtu, init, owner, identifier, context);
SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusRtu, handlePointer);

SDEVICE_PROPERTY_TYPE_DECLARATION(ModbusRtu, SlaveAddress, uint8_t);
SDEVICE_GET_SIMPLE_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, handle, value);
SDEVICE_SET_SIMPLE_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, handle, value);

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

bool ModbusRtuSDeviceTryProcessRequest(
      SDEVICE_HANDLE(ModbusRtu) *handle,
      ModbusRtuSDeviceInput      input,
      ModbusRtuSDeviceOutput     output);
