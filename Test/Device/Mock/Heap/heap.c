#include "heap.h"

#include "SDeviceCore/heap.h"

#include <stdlib.h>

void * SDeviceMalloc(size_t size)
{
   return malloc(size);
}

void SDeviceFree(void *_handle)
{
   free(_handle);
}
