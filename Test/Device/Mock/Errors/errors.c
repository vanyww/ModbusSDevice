#include "errors.h"

#include "SDeviceCore/errors.h"

static bool WasAssertFailedFlag = false;
static bool WasExceptionThrowedFlag = false;
static bool WasStatusLoggedFlag = false;

bool WasAssertFailed(void)
{
   bool state = WasAssertFailedFlag;
   WasAssertFailedFlag = false;
   return state;
}

bool WasExceptionThrowed(void)
{
   bool state = WasExceptionThrowedFlag;
   WasExceptionThrowedFlag = false;
   return state;
}

bool WasStatusLogged(void)
{
   bool state = WasStatusLoggedFlag;
   WasStatusLoggedFlag = false;
   return state;
}

bool AnyErrorDetected(void)
{
   return WasAssertFailed() || WasExceptionThrowed() || WasStatusLogged();
}

void SDeviceProcessAssertFail(char *, int)
{
   WasAssertFailedFlag = true;
}

void SDeviceProcessUnhandledThrow(CEXCEPTION_T handle)
{
   WasExceptionThrowedFlag = true;
}

void SDeviceProcessLogStatus(const void *)
{
   WasStatusLoggedFlag = true;
}
