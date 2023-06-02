#pragma once

#include "public_base.h"

#include <stdbool.h>

#define MODBUS_UDP_SDEVICE_MAX_MESSAGE_SIZE ((size_t)260)
#define MODBUS_UDP_SDEVICE_MAX_BTU_MESSAGE_SIZE (MODBUS_UDP_SDEVICE_BTU_ADDRESS_SIZE +                                 \
                                                 MODBUS_UDP_SDEVICE_MAX_MESSAGE_SIZE)

typedef struct
{
   bool    IsBroadcast;
   uint8_t SlaveAddress;
} ModbusUdpSDeviceOperationContext;

SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusUdp);
SDEVICE_INIT_DATA_FORWARD_DECLARATION(ModbusUdp);

typedef enum
{
   MODBUS_UDP_SDEVICE_STATUS_OK                   = MODBUS_SDEVICE_STATUS_OK,
   MODBUS_UDP_SDEVICE_STATUS_CORRUPTED_REQUEST    = MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST,
   MODBUS_UDP_SDEVICE_STATUS_WRONG_FUNCTION_CODE  = MODBUS_SDEVICE_STATUS_WRONG_FUNCTION_CODE,
   MODBUS_UDP_SDEVICE_STATUS_REGISTER_ACCESS_FAIL = MODBUS_SDEVICE_STATUS_REGISTERS_ACCESS_FAIL,
} ModbusUdpSDeviceStatus;

SDEVICE_INIT_DATA_DECLARATION(ModbusUdp)
{
   SDEVICE_INIT_DATA_DECLARATION(Modbus) BaseInit;
   const uint8_t *BtuAddress;
};

SDEVICE_STRING_NAME_DECLARATION(ModbusUdp);

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusUdp, init, owner, identifier, context);
SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusUdp, handlePointer);

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
