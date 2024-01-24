#pragma once

#include "ModbusSDevice/public_base.h"

#include "SDeviceCore/errors.h"

#define MAX_PDU_SIZE 253U
#define SWAP_UINT16_BYTES(value) __builtin_bswap16(value)

#define BI_ENDIAN_UINT16_FROM_NETWORK(value) (                                                                         \
   {                                                                                                                   \
      __auto_type _value = (value);                                                                                    \
      (BiEndianUInt16)                                                                                                 \
      {                                                                                                                \
         .AsNetwork = (_value),                                                                                        \
         .AsNative  = SWAP_UINT16_BYTES(_value)                                                                        \
      };                                                                                                               \
   })

#define BI_ENDIAN_UINT16_FROM_NATIVE(value) (                                                                          \
   {                                                                                                                   \
      __auto_type _value = (value);                                                                                    \
      (BiEndianUInt16)                                                                                                 \
      {                                                                                                                \
         .AsNetwork = SWAP_UINT16_BYTES(_value),                                                                       \
         .AsNative  = (_value)                                                                                         \
      };                                                                                                               \
   })

typedef struct
{
   uint16_t AsNative;
   uint16_t AsNetwork;
} BiEndianUInt16;

typedef struct
{
   const void *RequestData;
   const void *OperationContext;
   size_t      RequestSize;
   bool        IsOutputMandatory;
} ProcessingStageInput;

typedef struct
{
   void   *ResponseData;
   size_t *ResponseSize;
} ProcessingStageOutput;

typedef struct
{
   bool SupportsBroadcasts;
} BaseRuntimeData;

typedef ModbusSDeviceBaseInitData BaseInitData;
typedef ModbusSDeviceBaseBroadcastContext BaseBroadcastContext;
typedef ModbusSDeviceBaseProtocolException BaseProtocolException;
typedef ModbusSDeviceBaseReadOperationParameters BaseReadOperationParameters;
typedef ModbusSDeviceBaseWriteOperationParameters BaseWriteOperationParameters;

__attribute__((always_inline))
static inline bool HandleSupportsBroadcasts(void *handle)
{
   return ((BaseRuntimeData *)SDeviceGetHandleRuntimeData(handle))->SupportsBroadcasts;
}

bool ModbusSDeviceBaseTryProcessRequestPdu(void                 *handle,
                                           ProcessingStageInput  input,
                                           ProcessingStageOutput output);
