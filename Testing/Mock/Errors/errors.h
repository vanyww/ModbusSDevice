#pragma once

#include <stdbool.h>

#define WAS_ANY_ERROR_THROWN() (                                                                                       \
   {                                                                                                                   \
      bool _wasAnyErrorThrown = LastThrowedPanicHandle != NULL ||                                                      \
                                LastLogStatusHandle != NULL    ||                                                      \
                                WasAssertFailedCalled;                                                                 \
      LastThrowedPanicHandle = NULL;                                                                                   \
      LastLogStatusHandle = NULL;                                                                                      \
      WasAssertFailedCalled = false;                                                                                   \
      _wasAnyErrorThrown;                                                                                              \
   })

extern const void *LastThrowedPanicHandle;
extern const void *LastLogStatusHandle;
extern bool WasAssertFailedCalled;
