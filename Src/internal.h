#pragma once

#include "ModbusSDevice/public.h"

typedef struct
{
   const void *RequestData;
   const void *CallParameters;
   size_t      RequestSize;
   bool        IsOutputMandatory;
} ModbusSDevicePduProcessingStageInput;

typedef struct
{
   void   *ResponseData;
   size_t *ResponseSize;
} ModbusSDevicePduProcessingStageOutput;

bool ModbusSDeviceInternalTryProcessRequestPdu(
      void                                 *this,
      ModbusSDevicePduProcessingStageInput  input,
      ModbusSDevicePduProcessingStageOutput output);
