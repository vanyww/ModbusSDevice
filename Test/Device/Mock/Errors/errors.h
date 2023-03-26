#pragma once

#include <stdbool.h>

bool WasAssertFailed(void);
bool WasExceptionThrowed(void);
bool WasStatusLogged(void);

bool AnyErrorDetected(void);
