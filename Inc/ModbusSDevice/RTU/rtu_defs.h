#pragma once

#include "SDeviceCore/interface.h"

#include <stdint.h>

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
   ModbusSDeviceRtuRequestType ModbusSDeviceRtuRequestType;
} ModbusSDeviceRtuRequestData;

__SDEVICE_SET_SETTING_DECLARATION(Modbus, SlaveAddress,,);
