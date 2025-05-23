#pragma once

#include "config.h"
#include "dependencies.h"
#include "../public.h"

#define MODBUS_RTU_SDEVICE_VERSION_MAJOR MODBUS_SDEVICE_VERSION_MAJOR
#define MODBUS_RTU_SDEVICE_VERSION_MINOR MODBUS_SDEVICE_VERSION_MINOR
#define MODBUS_RTU_SDEVICE_VERSION_PATCH MODBUS_SDEVICE_VERSION_PATCH

#define MODBUS_RTU_SDEVICE_INTERCHAR_DELAY_CHARS  1.5f
#define MODBUS_RTU_SDEVICE_INTERFRAME_DELAY_CHARS 3.5f

#define MODBUS_RTU_SDEVICE_PREDEFINED_DELAYS_USAGE_THERESHOLD_BAUD 19200u
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

#define MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE 256u

SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusRtu);
SDEVICE_INIT_DATA_FORWARD_DECLARATION(ModbusRtu);

typedef struct
{
   bool IsBroadcast;
} ModbusRtuSDeviceCallParameters;

SDEVICE_INIT_DATA_DECLARATION(ModbusRtu)
{
   ModbusSDeviceInitData Base;

   uint16_t (* ComputeCrc16)(
         SDEVICE_HANDLE(ModbusRtu) *this,
         const void                *data,
         size_t                     size);
};

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusRtu, init, context);
SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusRtu, this);

SDEVICE_PROPERTY_TYPE_DECLARATION(ModbusRtu, SlaveAddress, uint8_t);
SDEVICE_GET_SIMPLE_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, this, value);
SDEVICE_SET_SIMPLE_PROPERTY_DECLARATION(ModbusRtu, SlaveAddress, this, value);

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
      SDEVICE_HANDLE(ModbusRtu) *this,
      ModbusRtuSDeviceInput      input,
      ModbusRtuSDeviceOutput     output);
