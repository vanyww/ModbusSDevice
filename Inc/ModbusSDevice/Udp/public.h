#pragma once

#include "ModbusSDevice/public_base.h"

#include "config.h"
#include "dependencies.h"

#define MODBUS_UDP_SDEVICE_VERSION_MAJOR MODBUS_SDEVICE_BASE_VERSION_MAJOR
#define MODBUS_UDP_SDEVICE_VERSION_MINOR MODBUS_SDEVICE_BASE_VERSION_MINOR
#define MODBUS_UDP_SDEVICE_VERSION_PATCH MODBUS_SDEVICE_BASE_VERSION_PATCH

#define MODBUS_UDP_SDEVICE_MAX_MESSAGE_SIZE 260U
#define MODBUS_UDP_SDEVICE_MAX_BTU_MESSAGE_SIZE                                                                        \
   (MODBUS_UDP_SDEVICE_BTU_ADDRESS_SIZE + MODBUS_UDP_SDEVICE_MAX_MESSAGE_SIZE)

typedef struct
{
   ModbusSDeviceBaseBroadcastContext Base;
   uint8_t                           SlaveAddress;
} ModbusUdpSDeviceCallParameters;

SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusUdp);
SDEVICE_INIT_DATA_FORWARD_DECLARATION(ModbusUdp);

SDEVICE_INIT_DATA_DECLARATION(ModbusUdp)
{
   ModbusSDeviceBaseInitData Base;
};

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusUdp, init, context);
SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusUdp, handlePointer);

SDEVICE_PROPERTY_TYPE_DECLARATION(ModbusUdp, BtuAddress, uint8_t)[MODBUS_UDP_SDEVICE_BTU_ADDRESS_SIZE];
SDEVICE_GET_SIMPLE_PROPERTY_DECLARATION(ModbusUdp, BtuAddress, handle, value);
SDEVICE_SET_SIMPLE_PROPERTY_DECLARATION(ModbusUdp, BtuAddress, handle, value);

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

bool ModbusUdpSDeviceTryProcessRequest(
      SDEVICE_HANDLE(ModbusUdp) *handle,
      ModbusUdpSDeviceInput      input,
      ModbusUdpSDeviceOutput     output);
