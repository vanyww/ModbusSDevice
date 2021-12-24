#pragma once

#include "common.h"

#include <math.h>

#define __MODBUS_RTU_SDEVICE_INTERCHAR_DELAY_CHARS 1.5f
#define __MODBUS_RTU_SDEVICE_INTERFRAME_DELAY_CHARS 3.5f

#define __MODBUS_RTU_SDEVICE_PREDEFINED_DELAYS_USAGE_THERESHOLD_BAUD 19200
#define __MODBUS_RTU_SDEVICE_INTERCHAR_DELAY_PREDEFINED 0.750e-3f
#define __MODBUS_RTU_SDEVICE_INTERFRAME_DELAY_PREDEFINED 1.750e-3f

#define __MODBUS_RTU_SDEVICE_CHARS_TO_SECONDS_DELAY(baud, chars, char_size) (((char_size) * (chars)) / (baud))

#define __MODBUS_RTU_SDEVICE_GET_INTERCHAR_DELAY(baud, char_size)                                                      \
    (((baud) > __MODBUS_SDEVICE_RTU_PREDEFINED_DELAYS_USAGE_THERESHOLD_BAUD) ?                                         \
      __MODBUS_SDEVICE_RTU_INTERCHAR_DELAY_PREDEFINED                        :                                         \
      __MODBUS_SDEVICE_RTU_CHARS_TO_SECONDS_DELAY(baud, __MODBUS_SDEVICE_RTU_INTERCHAR_DELAY_CHARS, char_size))
#define __MODBUS_RTU_SDEVICE_GET_INTERFRAME_DELAY(baud, char_size)                                                     \
    (((baud) > __MODBUS_SDEVICE_RTU_PREDEFINED_DELAYS_USAGE_THERESHOLD_BAUD) ?                                         \
      __MODBUS_SDEVICE_RTU_INTERFRAME_DELAY_PREDEFINED                       :                                         \
      __MODBUS_SDEVICE_RTU_CHARS_TO_SECONDS_DELAY(baud, __MODBUS_SDEVICE_RTU_INTERFRAME_DELAY_CHARS, char_size))

#define __MODBUS_RTU_SDEVICE_MAX_MESSAGE_SIZE 256U /* 253 (max PDU) + 3 (common ADU members) */
#define __MODBUS_RTU_SDEVICE_MAX_VALID_SLAVE_ADDRESS 247U
#define __MODBUS_RTU_SDEVICE_BROADCAST_REQUEST_SLAVE_ADDRESS 0U
#define __MODBUS_RTU_SDEVICE_IS_VALID_SLAVE_ADDRESS(address) (                                                         \
{                                                                                                                      \
   __typeof__(address) _address = address;                                                                             \
   _address != __MODBUS_RTU_SDEVICE_BROADCAST_REQUEST_SLAVE_ADDRESS &&                                                 \
   _address <= __MODBUS_RTU_SDEVICE_MAX_VALID_SLAVE_ADDRESS;                                                           \
})

typedef enum
{
   MODBUS_RTU_SDEVICE_REQUEST_TYPE_NORMAL,
   MODBUS_RTU_SDEVICE_REQUEST_TYPE_BROADCAST
} ModbusRtuSDeviceRequestType;

typedef struct
{
   ModbusSDeviceRequestData Common;
   ModbusRtuSDeviceRequestType RequestType;
} ModbusRtuSDeviceRequestData;

/* Satty's interface start */

__SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusRtu);

typedef struct
{
   __SDEVICE_CONSTANT_DATA(Modbus) ModbusCommon;
} __SDEVICE_CONSTANT_DATA(ModbusRtu);

typedef struct
{
   uint8_t SlaveAddress;
} ModbusRtuSDeviceRtuSettings;

typedef struct
{
   uint8_t SlaveAddress;
} __SDEVICE_SETTINGS_DATA(ModbusRtu);

typedef struct { } __SDEVICE_DYNAMIC_DATA(ModbusRtu);

__SDEVICE_HANDLE_DEFINITION(ModbusRtu);

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(ModbusRtu,);

__SDEVICE_SET_SETTING_DECLARATION(ModbusRtu, SlaveAddress,,);

/* Satty's interface end */

bool ModbusRtuSDeviceTryProcessRequest(__SDEVICE_HANDLE(ModbusRtu) *, ModbusSDeviceRequest *, ModbusSDeviceResponse *);