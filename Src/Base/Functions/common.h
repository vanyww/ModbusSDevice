#pragma once

#include "03Function/data_types.h"
#include "16Function/data_types.h"

typedef union __attribute__((__may_alias__))
{
   Function03Request AsFunction03Request;
   Function16Request AsFunction16Request;
} FunctionRequest;

typedef union __attribute__((__may_alias__))
{
   Function03Response AsFunction03Response;
   Function16Response AsFunction16Response;
} FunctionResponse;

typedef struct
{
   const FunctionRequest *Request;
   size_t                 RequestSize;
} FunctionInput;

typedef struct
{
   FunctionResponse *Response;
   size_t           *ResponseSize;
} FunctionOutput;
