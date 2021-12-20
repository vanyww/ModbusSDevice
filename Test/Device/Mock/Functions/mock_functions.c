#include "mock_functions.h"

#include <memory.h>

ModbusSDeviceRegister MockReadRegisters[__MOCK_REGISTERS_COUNT];
ModbusSDeviceRegister MockWriteRegisters[__MOCK_REGISTERS_COUNT];

ModbusSDeviceStatus ReadRegistersMock(SDeviceCommonHandle *handle,
                                      ModbusSDeviceRegister *registers,
                                      const ModbusSDeviceOperationParameters *parameters)
{
   if(parameters->RegisterAddress + parameters->RegistersCount > __MOCK_REGISTERS_COUNT)
      return MODBUS_SDEVICE_STATUS_ILLEGAL_ADDRESS_ERROR;

   memcpy(registers,
          &MockReadRegisters[parameters->RegisterAddress],
          parameters->RegistersCount * sizeof(ModbusSDeviceRegister));

   return MODBUS_SDEVICE_STATUS_OK;
}

ModbusSDeviceStatus WriteRegistersMock(SDeviceCommonHandle *handle,
                                       const ModbusSDeviceRegister *registers,
                                       const ModbusSDeviceOperationParameters *parameters)
{
   if(parameters->RegisterAddress + parameters->RegistersCount > __MOCK_REGISTERS_COUNT)
      return MODBUS_SDEVICE_STATUS_ILLEGAL_ADDRESS_ERROR;

   memcpy(&MockWriteRegisters[parameters->RegisterAddress],
          registers,
          parameters->RegistersCount * sizeof(ModbusSDeviceRegister));

   return MODBUS_SDEVICE_STATUS_OK;
}
