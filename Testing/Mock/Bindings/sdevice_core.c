#include "sdevice_core.h"

#include "SDeviceCore/common.h"
#include "CoreGlobalSDevice/public.h"

#include "unity_fixture.h"

#include <stdlib.h>

AssertFailHandler ThisAssertFailHandler;
PanicHandler ThisPanicHandler;

void SetAssertFailHandler(AssertFailHandler handler)
{
   ThisAssertFailHandler = handler;
}

void SetPanicHandler(PanicHandler handler)
{
   ThisPanicHandler = handler;
}

void ResetAssertFailHandler(void)
{
   ThisAssertFailHandler = NULL;
}

void ResetPanicHandler(void)
{
   ThisPanicHandler = NULL;
}

void * SDeviceAllocateMemory(size_t size)
{
   if(size <= 0)
      return NULL;

   void *memoryPointer = malloc(size);

   if(!memoryPointer)
      CoreGlobalSDeviceThrowPanic(CORE_GLOBAL_SDEVICE_PANIC_OUT_OF_MEMORY);

   return memoryPointer;
}

void SDeviceFreeMemory(void *pointer)
{
   free(pointer);
}

void SDeviceProcessAssertFail(void)
{
   if(ThisAssertFailHandler)
   {
      ThisAssertFailHandler();
   }
   else
   {
      TEST_FAIL();
   }
}

void SDeviceProcessPanic(const void *handle, SDevicePanic panic)
{
   if(ThisPanicHandler)
   {
      ThisPanicHandler(handle, panic);
   }
   else
   {
      TEST_FAIL();
   }
}
