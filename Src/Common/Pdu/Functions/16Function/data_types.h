#pragma once

#include "../../data_types.h"

typedef struct __attribute__((packed, scalar_storage_order("big-endian")))
{
   uint16_t RegistersAddress;
   uint16_t RegistersCount;
   uint8_t BytesToFollow;
   uint8_t RegistersBuffer[];
} Function16Request;

typedef struct __attribute__((packed, scalar_storage_order("big-endian")))
{
   uint16_t RegistersAddress;
   uint16_t RegistersCount;
} Function16Response;
