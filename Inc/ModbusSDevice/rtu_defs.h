#pragma once

#include "SDeviceCore/interface.h"

#include <stdint.h>
#include <math.h>

#define __MODBUS_SDEVICE_RTU_INTERCHAR_DELAY_CHARS 1.5f
#define __MODBUS_SDEVICE_RTU_INTERFRAME_DELAY_CHARS 3.5f

#define __MODBUS_SDEVICE_RTU_PREDEFINED_DELAYS_USAGE_THERESHOLD_BAUD 19200
#define __MODBUS_SDEVICE_RTU_INTERCHAR_DELAY_PREDEFINED 0.750e-3f
#define __MODBUS_SDEVICE_RTU_INTERFRAME_DELAY_PREDEFINED 1.750e-3f

#define __MODBUS_SDEVICE_RTU_CHARS_TO_SECONDS_DELAY(baud, chars, char_size) (((char_size) * (chars)) / (baud))

#define __MODBUS_SDEVICE_RTU_GET_INTERCHAR_DELAY(baud, char_size)                                                      \
    (((baud) > __MODBUS_SDEVICE_RTU_PREDEFINED_DELAYS_USAGE_THERESHOLD_BAUD) ?                                         \
      __MODBUS_SDEVICE_RTU_INTERCHAR_DELAY_PREDEFINED                        :                                         \
      __MODBUS_SDEVICE_RTU_CHARS_TO_SECONDS_DELAY(baud, __MODBUS_SDEVICE_RTU_INTERCHAR_DELAY_CHARS, char_size))
#define __MODBUS_SDEVICE_RTU_GET_INTERFRAME_DELAY(baud, char_size)                                                     \
    (((baud) > __MODBUS_SDEVICE_RTU_PREDEFINED_DELAYS_USAGE_THERESHOLD_BAUD) ?                                         \
      __MODBUS_SDEVICE_RTU_INTERFRAME_DELAY_PREDEFINED                       :                                         \
      __MODBUS_SDEVICE_RTU_CHARS_TO_SECONDS_DELAY(baud, __MODBUS_SDEVICE_RTU_INTERFRAME_DELAY_CHARS, char_size))

#define __MODBUS_SDEVICE_RTU_TIMER_DIVIDER 1000000
#define __MODBUS_SDEVICE_RTU_TIMER_PERIOD (1.f / __MODBUS_SDEVICE_RTU_TIMER_DIVIDER)

/* gcc's ceilf is compiler intrinsic, so we can use it as constant value */
#define __MODBUS_SDEVICE_RTU_DELAY_TO_TIMER_PERIOD(delay) (ceilf((delay) * __MODBUS_SDEVICE_RTU_TIMER_DIVIDER) - 1)
#define __MODBUS_SDEVICE_RTU_GET_TIMER_PRESCALER(clock) ((clock) / __MODBUS_SDEVICE_RTU_TIMER_DIVIDER - 1)

#define __MODBUS_SDEVICE_RTU_MAX_MESSAGE_SIZE 256U /* 253 (max PDU) + 3 (common ADU members) */
#define __MODBUS_SDEVICE_MAX_VALID_SLAVE_ADDRESS 247U
#define __MODBUS_SDEVICE_BROADCAST_REQUEST_SLAVE_ADDRESS 0U
#define __MODBUS_RTU_IS_VALID_SLAVE_ADDRESS(address) (                                                                 \
{                                                                                                                      \
   __typeof__(address) _address = address;                                                                             \
   _address != __MODBUS_SDEVICE_BROADCAST_REQUEST_SLAVE_ADDRESS &&                                                     \
   _address <= __MODBUS_SDEVICE_MAX_VALID_SLAVE_ADDRESS;                                                               \
})

typedef enum
{
   MODBUS_SDEVICE_RTU_REQUEST_TYPE_NORMAL,
   MODBUS_SDEVICE_RTU_REQUEST_TYPE_BROADCAST
} ModbusSDeviceRtuRequestType;

typedef struct
{
   ModbusSDeviceRtuRequestType RequestType;
} ModbusSDeviceRtuRequestData;

__SDEVICE_SET_SETTING_DECLARATION(Modbus, SlaveAddress,,);
