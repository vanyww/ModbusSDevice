#pragma once

#include "ModbusSDevice/public_base.h"

#include "config.h"
#include "dependencies.h"
#include "log.h"

/* 5E168276-5933-11EE-A30E-B3F58C691376 */
#define MODBUS_TCP_SDEVICE_UUID_HIGH 0x5E168276593311EE
#define MODBUS_TCP_SDEVICE_UUID_LOW  0xA30EB3F58C691376

#define MODBUS_TCP_SDEVICE_VERSION_MAJOR MODBUS_SDEVICE_BASE_VERSION_MAJOR
#define MODBUS_TCP_SDEVICE_VERSION_MINOR MODBUS_SDEVICE_BASE_VERSION_MINOR
#define MODBUS_TCP_SDEVICE_VERSION_PATCH MODBUS_SDEVICE_BASE_VERSION_PATCH

#define MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE 7U
#define MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE 253U

typedef struct
{
   uint8_t SlaveAddress;
} ModbusTcpSDeviceCallParameters;

SDEVICE_HANDLE_FORWARD_DECLARATION(ModbusTcp);
SDEVICE_INIT_DATA_FORWARD_DECLARATION(ModbusTcp);

SDEVICE_INIT_DATA_DECLARATION(ModbusTcp)
{
   ModbusSDeviceBaseInitData Base;
};

SDEVICE_IDENTITY_BLOCK_DECLARATION(ModbusTcp);

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

bool ModbusTcpSDeviceTryProcessMbapHeader(
      SDEVICE_HANDLE(ModbusTcp) *handle,
      const void                *mbapHeaderData,
      size_t                    *leftPacketSize);

bool ModbusTcpSDeviceTryProcessRequest(
      SDEVICE_HANDLE(ModbusTcp) *handle,
      ModbusTcpSDeviceInput      input,
      ModbusTcpSDeviceOutput     output);
