#pragma once

#include "ModbusSDevice/public_base.h"
#include "Functions/common.h"

#include <stdbool.h>

#define MAX_PDU_SIZE 253U

typedef struct __attribute__((packed, __may_alias__))
{
   uint8_t FunctionCode;
   union __attribute__((__may_alias__))
   {
      FunctionRequest  AsFunctionRequest;
      FunctionResponse AsFunctionResponse;

      struct __attribute__((packed, __may_alias__))
      {
         uint8_t ExceptionCode;
      } AsExceptionResponse;
   } Data;
} MessagePdu;

typedef struct
{
   const MessagePdu *Pdu;
   size_t            PduSize;
} PduInput;

typedef struct
{
   MessagePdu *Pdu;
   size_t     *PduSize;
} PduOutput;

SDEVICE_CREATE_HANDLE_DECLARATION(Modbus, init, owner, identifier, context);
SDEVICE_DISPOSE_HANDLE_DECLARATION(Modbus, handlePointer);

bool ModbusSDeviceTryProcessRequestPdu(SDEVICE_HANDLE(Modbus) *handle,
                                       const void             *operationContext,
                                       PduInput                input,
                                       PduOutput               output);
