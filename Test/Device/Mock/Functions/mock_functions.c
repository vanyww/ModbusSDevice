#include "mock_functions.h"

#include <memory.h>

ModbusRegister MockReadRegisters[__MOCK_REGISTERS_COUNT];
ModbusRegister MockWriteRegisters[__MOCK_REGISTERS_COUNT];

ModbusStatus ReadRegistersMock(SDeviceCommonHandle *handle,
                               ModbusRegister *registers,
                               const ModbusOperationParameters *parameters)
{
   if(parameters->RegisterAddress + parameters->RegistersCount > __MOCK_REGISTERS_COUNT)
      return MODBUS_STATUS_ILLEGAL_ADDRESS_ERROR;

   memcpy(registers,
          &MockReadRegisters[parameters->RegisterAddress],
          parameters->RegistersCount * sizeof(ModbusRegister));

   return MODBUS_STATUS_OK;
}

ModbusStatus WriteRegistersMock(SDeviceCommonHandle *handle,
                                const ModbusRegister *registers,
                                const ModbusOperationParameters *parameters)
{
   if(parameters->RegisterAddress + parameters->RegistersCount > __MOCK_REGISTERS_COUNT)
      return MODBUS_STATUS_ILLEGAL_ADDRESS_ERROR;

   memcpy(&MockWriteRegisters[parameters->RegisterAddress],
          registers,
          parameters->RegistersCount * sizeof(ModbusRegister));

   return MODBUS_STATUS_OK;
}
