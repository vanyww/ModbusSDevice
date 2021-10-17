#include "Tests/Initialization/test_initialization.h"
#include "Tests/Settings/test_settings.h"
#include "Tests/Requests/RTU/Read/test_rtu_read.h"
#include "Tests/Requests/RTU/Write/test_rtu_write.h"
#include "Tests/Requests/TCP/Read/test_tcp_read.h"
#include "Tests/Requests/TCP/Write/test_tcp_write.h"

#include <stdio.h>
#include <string.h>

struct
{
   bool (*Function)(void);
   char *Description;
} TestFunctions[] =
{
   { TestRtuHandleInitialization, "rtu handle initialization" },
   { TestTcpHandleInitialization, "tcp handle initialization" },
   { TestSlaveAddressSettingSet, "rtu handle set slave address setting" },

   { TestRtuReadOneRequest, "rtu read one register" },
   { TestRtuReadMultipleRequest, "rtu read multiple registers" },
   { TestRtuReadTooManyRequest, "rtu read too many registers" },
   { TestRtuReadWithWrongSlaveAddressRequest, "rtu read with wrong slave address" },
   { TestRtuWriteOneRequest, "rtu write one register" },
   { TestRtuWriteMultipleRequest, "rtu write multiple registers" },

   { TestTcpReadOneRequest, "tcp read one register" },
   { TestTcpReadMultipleRequest, "tcp read multiple registers" },
   { TestTcpReadTooManyRequest, "tcp read too many registers" },
   { TestTcpWriteOneRequest, "tcp write one register" },
   { TestTcpWriteMultipleRequest, "tcp write multiple registers" }
};

const size_t TestFunctionsCount = sizeof(TestFunctions) / sizeof(*TestFunctions);

int main(void)
{
   int maxMessageLength = 0;
   for(size_t i = 0; i < TestFunctionsCount; i++)
   {
      int length = strlen(TestFunctions[i].Description);
      if(length > maxMessageLength)
         maxMessageLength = length;
   }

   printf("start tests...\n\n");

   for(size_t i = 0; i < TestFunctionsCount; i++)
      printf("%*s - %s\n",
             -maxMessageLength, TestFunctions[i].Description, TestFunctions[i].Function() == true ? "pass" : "fail");

   printf("\nstop tests...\n");
}