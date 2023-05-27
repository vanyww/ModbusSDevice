#pragma once

#include <stdint.h>

typedef struct __attribute__((packed, __may_alias__))
{
   uint16_t RegistersAddress;
   uint16_t RegistersCount;
} Function03Request;

typedef struct __attribute__((packed, __may_alias__))
{
   uint8_t FollowingDataBytes;
   uint8_t RegistersData[];
} Function03Response;
