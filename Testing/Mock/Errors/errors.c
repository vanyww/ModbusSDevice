#include "errors.h"

#include "SDeviceCore/errors.h"

const void *LastThrowedPanicHandle;
const void *LastLogStatusHandle;
bool WasAssertFailedCalled;

void SDeviceProcessAssertFail(void)
{
   WasAssertFailedCalled = true;
}

void SDeviceProcessPanic(const void *_handle)
{
   LastThrowedPanicHandle = _handle;
}

void SDeviceProcessLogStatus(const void *_handle, const void *extras, size_t extrasSize)
{
   LastLogStatusHandle = _handle;
}
