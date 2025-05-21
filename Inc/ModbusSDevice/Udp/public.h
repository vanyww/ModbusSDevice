
#pragma once

#include "config.h"
#include "dependencies.h"
#include "../public.h"

#define MODBUS_UDP_SDEVICE_VERSION_MAJOR MODBUS_SDEVICE_VERSION_MAJOR
#define MODBUS_UDP_SDEVICE_VERSION_MINOR MODBUS_SDEVICE_VERSION_MINOR
#define MODBUS_UDP_SDEVICE_VERSION_PATCH MODBUS_SDEVICE_VERSION_PATCH

#define MODBUS_UDP_SDEVICE_MAX_MESSAGE_SIZE 260u
#define MODBUS_UDP_SDEVICE_MAX_BTU_MESSAGE_SIZE                                                                        \
   (MODBUS_UDP_SDEVICE_BTU_ADDRESS_SIZE + MODBUS_UDP_SDEVICE_MAX_MESSAGE_SIZE)

SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusUdp);
SDEVICE_INIT_DATA_FORWARD_DECLARATION(ModbusUdp);

typedef struct
{
   uint8_t SlaveAddress;
   bool    IsBroadcast;
} ModbusUdpSDeviceCallParameters;

typedef struct
{
   uint8_t Values[MODBUS_UDP_SDEVICE_BTU_ADDRESS_SIZE];
} ModbusUdpSDeviceBtuAddress;

SDEVICE_INIT_DATA_DECLARATION(ModbusUdp)
{
   ModbusSDeviceInitData Base;
};

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusUdp, init, context);
SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusUdp, this);

SDEVICE_PROPERTY_TYPE_DECLARATION(ModbusUdp, BtuAddress, ModbusUdpSDeviceBtuAddress);
SDEVICE_GET_SIMPLE_PROPERTY_DECLARATION(ModbusUdp, BtuAddress, this, value);
SDEVICE_SET_SIMPLE_PROPERTY_DECLARATION(ModbusUdp, BtuAddress, this, value);

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
      SDEVICE_HANDLE(ModbusUdp) *this,
      ModbusUdpSDeviceInput      input,
      ModbusUdpSDeviceOutput     output);
