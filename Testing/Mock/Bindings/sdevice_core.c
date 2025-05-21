#include "sdevice_core.h"

#include "unity_fixture.h"

#include <stdlib.h>
#include <stdio.h>

#define ASSERT_FAIL_FORMAT "Assert failed on file (%s) line (%d)"

AssertFailHandler ThisAssertFailHandler;

void SetAssertFailHandler(AssertFailHandler handler)
{
   ThisAssertFailHandler = handler;
}

void ResetAssertFailHandler(void)
{
   ThisAssertFailHandler = NULL;
}

void * SDeviceAllocateMemory(size_t size)
{
   return (size <= 0) ? NULL : malloc(size);
}

void SDeviceFreeMemory(void *pointer)
{
   free(pointer);
}

void * SDeviceAllocateHandle(size_t initSize, size_t runtimeSize)
{
   SDeviceCommonHandle *handle =
         SDeviceAllocateMemory(
               sizeof(*handle));

   handle->Init = SDeviceAllocateMemory(initSize);
   handle->Runtime = SDeviceAllocateMemory(runtimeSize);

   return handle;
}

void SDeviceFreeHandle(void *handle)
{
   SDeviceCommonHandle *_handle = handle;

   SDeviceFreeMemory(_handle->Runtime);
   SDeviceFreeMemory(_handle->Init);
   SDeviceFreeMemory(_handle);
}

void SDeviceProcessAssertFail(char *file, int line)
{
   int messageLength = snprintf(NULL, 0, ASSERT_FAIL_FORMAT, file, line);
   char message[messageLength + 1];

   sprintf(message, ASSERT_FAIL_FORMAT, file, line);

   TEST_MESSAGE(message);

   if(ThisAssertFailHandler)
   {
      ThisAssertFailHandler();
   }
   else
   {
      TEST_FAIL_MESSAGE("Assert fail has not been processed");
   }
}
