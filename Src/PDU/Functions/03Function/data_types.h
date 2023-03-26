#pragma once

#include "../../data_types.h"

typedef struct __attribute__((packed, scalar_storage_order("big-endian")))
{
   uint16_t RegistersAddress;
   uint16_t RegistersCount;
} Function03Request;

typedef struct __attribute__((packed, scalar_storage_order("big-endian")))
{
   uint8_t FollowingDataBytes;
   uint8_t RegistersBuffer[];
} Function03Response;
