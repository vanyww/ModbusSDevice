#pragma once

#include "public_base.h"

#include <stdbool.h>

#define MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE 7U
#define MODBUS_TCP_SDEVICE_MAX_REQUEST_SIZE 253U

typedef struct
{
   uint8_t SlaveAddress;
} ModbusTcpSDeviceOperationContext;

SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusTcp);
SDEVICE_INIT_DATA_FORWARD_DECLARATION(ModbusTcp);

typedef enum
{
   MODBUS_TCP_SDEVICE_STATUS_OK                   = MODBUS_SDEVICE_STATUS_OK,
   MODBUS_TCP_SDEVICE_STATUS_CORRUPTED_REQUEST    = MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST,
   MODBUS_TCP_SDEVICE_STATUS_WRONG_FUNCTION_CODE  = MODBUS_SDEVICE_STATUS_WRONG_FUNCTION_CODE,
   MODBUS_TCP_SDEVICE_STATUS_REGISTER_ACCESS_FAIL = MODBUS_SDEVICE_STATUS_REGISTERS_ACCESS_FAIL,
} ModbusTcpSDeviceStatus;

SDEVICE_INIT_DATA_DECLARATION(ModbusTcp)
{
   SDEVICE_INIT_DATA_DECLARATION(Modbus) BaseInit;
};

SDEVICE_STRING_NAME_DECLARATION(ModbusTcp);

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusTcp, init, owner, identifier, context);
SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusTcp, handlePointer);

typedef struct
{
   const void *RequestData;
   size_t      RequestSize;
} ModbusTcpSDeviceInput;

typedef struct
{
   void   *ResponseData;
   size_t *ResponseSize;
} ModbusTcpSDeviceOutput;

bool ModbusTcpSDeviceTryProcessMbapHeader(SDEVICE_HANDLE(ModbusTcp) *handle,
                                          const void                *mbapHeaderData,
                                          size_t                    *requestSizeToReceive);
bool ModbusTcpSDeviceTryProcessRequest(SDEVICE_HANDLE(ModbusTcp) *handle,
                                       ModbusTcpSDeviceInput      input,
                                       ModbusTcpSDeviceOutput     output);
