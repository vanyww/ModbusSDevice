#pragma once

#include "ModbusSDevice/public_base.h"

#include "dependencies.h"
#include "config.h"
#include "log.h"

/* 4ca2eb7b-5933-11ee-a1ab-5db02dbf0e99 */
#define MODBUS_UDP_SDEVICE_UUID_HIGH 0x4ca2eb7b593311ee
#define MODBUS_UDP_SDEVICE_UUID_LOW  0xa1ab5db02dbf0e99

#define MODBUS_UDP_SDEVICE_VERSION_MAJOR MODBUS_SDEVICE_BASE_VERSION_MAJOR
#define MODBUS_UDP_SDEVICE_VERSION_MINOR MODBUS_SDEVICE_BASE_VERSION_MINOR
#define MODBUS_UDP_SDEVICE_VERSION_PATCH MODBUS_SDEVICE_BASE_VERSION_PATCH

#define MODBUS_UDP_SDEVICE_MAX_MESSAGE_SIZE 260U
#define MODBUS_UDP_SDEVICE_MAX_BTU_MESSAGE_SIZE (MODBUS_UDP_SDEVICE_BTU_ADDRESS_SIZE +                                 \
                                                 MODBUS_UDP_SDEVICE_MAX_MESSAGE_SIZE)

typedef struct
{
   ModbusSDeviceBaseBroadcastContext Base;
   uint8_t                           SlaveAddress;
} ModbusUdpSDeviceOperationContext;

SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusUdp);
SDEVICE_INIT_DATA_FORWARD_DECLARATION(ModbusUdp);

SDEVICE_INIT_DATA_DECLARATION(ModbusUdp)
{
   ModbusSDeviceBaseInitData Base;
};

SDEVICE_IDENTITY_BLOCK_DECLARATION(ModbusUdp);

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusUdp, init, owner, identifier, context);
SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusUdp, handlePointer);

SDEVICE_PROPERTY_TYPE_DECLARATION(ModbusUdp, BtuAddress, uint8_t)[MODBUS_UDP_SDEVICE_BTU_ADDRESS_SIZE];
SDEVICE_GET_PROPERTY_DECLARATION(ModbusUdp, BtuAddress, handle, value);
SDEVICE_SET_PROPERTY_DECLARATION(ModbusUdp, BtuAddress, handle, value);

typedef struct
{
   const void *RequestData;
   size_t      RequestSize;
   bool        IsBroadcast;
} ModbusUdpSDeviceInput;

typedef struct
{
   void   *ResponseData;
   size_t *ResponseSize;
} ModbusUdpSDeviceOutput;

bool ModbusUdpSDeviceTryProcessRequest(SDEVICE_HANDLE(ModbusUdp) *handle,
                                       ModbusUdpSDeviceInput      input,
                                       ModbusUdpSDeviceOutput     output);
