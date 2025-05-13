#pragma once

#include "SDeviceCore/core.h"

typedef void (* AssertFailHandler)(void);

void SetAssertFailHandler(AssertFailHandler handler);
void ResetAssertFailHandler(void);
