#pragma once

#include "../private.h"

typedef struct __attribute__((packed, may_alias))
{
   uint8_t ExceptionCode;
} ExceptionFunctionResponse;

__attribute__((always_inline))
static inline void ProcessExceptionFunction(BaseProtocolException exception, ProcessingStageOutput output)
{
   ExceptionFunctionResponse *response = output.ResponseData;

   response->ExceptionCode = exception;

   *output.ResponseSize = sizeof(ExceptionFunctionResponse);
}
