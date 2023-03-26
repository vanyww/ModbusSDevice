#pragma once

#include "public_common.h"

#include <stdbool.h>

#define MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE 7U
#define MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE 260U /* 253 (max PDU) + 7 (ADU members) */

typedef struct
{
   ModbusSDeviceRequestContext Common;
   uint8_t SlaveAddress;
} ModbusTcpSDeviceRequestContext;

typedef enum
{
   MODBUS_TCP_SDEVICE_STATUS_OK                          = MODBUS_SDEVICE_STATUS_OK,
   MODBUS_TCP_SDEVICE_STATUS_CORRUPTED_REQUEST           = MODBUS_SDEVICE_STATUS_CORRUPTED_REQUEST,
   MODBUS_TCP_SDEVICE_STATUS_REGISTER_ACCESS_FAIL        = MODBUS_SDEVICE_STATUS_REGISTER_ACCESS_FAIL,
   MODBUS_TCP_SDEVICE_STATUS_REGISTERS_COUNT_MISMATCH    = MODBUS_SDEVICE_STATUS_REGISTERS_COUNT_MISMATCH,
   MODBUS_TCP_SDEVICE_STATUS_UNIMPLEMENTED_FUNCTION_CODE = MODBUS_SDEVICE_STATUS_UNIMPLEMENTED_FUNCTION_CODE,
} ModbusTcpSDeviceStatus;

SDEVICE_INIT_DATA_FORWARD_DECLARATION(ModbusTcp);

SDEVICE_INIT_DATA_DECLARATION(ModbusTcp)
{
   ModbusSDeviceRegistersCallbacks RegistersCallbacks;
};

SDEVICE_CREATE_HANDLE_DECLARATION(ModbusTcp, init, parent, identifier, context);
SDEVICE_DISPOSE_HANDLE_DECLARATION(ModbusTcp, handlePointer);

bool ModbusTcpSDeviceTryProcessMbapHeader(SDEVICE_HANDLE(ModbusTcp) *handle,
                                          const ModbusSDeviceRequest *header,
                                          size_t *lengthToReceive);
bool ModbusTcpSDeviceTryProcessRequest(SDEVICE_HANDLE(ModbusTcp) *handle,
                                       const ModbusSDeviceRequest *request,
                                       ModbusSDeviceResponse *response);
