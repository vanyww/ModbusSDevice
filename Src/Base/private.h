#pragma once

#include "ModbusSDevice/public_base.h"

#define MAX_PDU_SIZE 253U
#define SWAP_UINT16_BYTES(value) __builtin_bswap16(value)

typedef struct
{
   const void *RequestData;
   const void *CallParameters;
   size_t      RequestSize;
   bool        IsOutputMandatory;
} PduProcessingStageInput;

typedef struct
{
   void   *ResponseData;
   size_t *ResponseSize;
} PduProcessingStageOutput;

typedef struct
{
   bool SupportsBroadcasting;
} BaseRuntimeData;

typedef ModbusSDeviceBaseInitData ThisBaseInitData;
typedef ModbusSDeviceBaseBroadcastContext ThisBaseBroadcastContext;
typedef ModbusSDeviceBaseProtocolException ThisBaseProtocolException;

typedef ModbusSDeviceBaseReadOperationParameters ThisBaseReadOperationParameters;
typedef ModbusSDeviceBaseWriteOperationParameters ThisBaseWriteOperationParameters;

__attribute__((always_inline))
static inline bool SupportsBroadcasting(void *handle)
{
   return ((BaseRuntimeData *)SDeviceGetHandleRuntimeData(handle))->SupportsBroadcasting;
}

bool ModbusSDeviceBaseTryProcessRequestPdu(
      void                    *handle,
      PduProcessingStageInput  input,
      PduProcessingStageOutput output);
