#pragma once

#include <stdint.h>

typedef struct __attribute__((packed, __may_alias__))
{
   uint16_t RegistersAddress;
   uint16_t RegistersCount;
   uint8_t  BytesToFollow;
   uint8_t  RegistersData[];
} Function16Request;

typedef struct __attribute__((packed, __may_alias__))
{
   uint16_t RegistersAddress;
   uint16_t RegistersCount;
} Function16Response;
