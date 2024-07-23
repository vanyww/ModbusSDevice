#pragma once

#include "../private.h"

typedef struct __attribute__((packed))
{
   uint8_t ExceptionCode;
} __attribute__((may_alias)) ExceptionFunctionResponse;

__attribute__((always_inline))
static inline void ProcessExceptionFunction(ThisBaseProtocolException exception, PduProcessingStageOutput output)
{
   ExceptionFunctionResponse *response = output.ResponseData;

   response->ExceptionCode = exception;

   *output.ResponseSize = sizeof(ExceptionFunctionResponse);
}
