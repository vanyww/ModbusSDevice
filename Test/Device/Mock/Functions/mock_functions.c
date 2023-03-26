#include "mock_functions.h"

#include <memory.h>

ModbusSDeviceRegister MockReadRegisters[MOCK_REGISTERS_COUNT];
ModbusSDeviceRegister MockWriteRegisters[MOCK_REGISTERS_COUNT];

ModbusSDeviceProtocolException ReadRegistersMock(ModbusSDeviceCommonHandle handle,
                                                 const ModbusSDeviceRegistersReadParameters *parameters)
{
   if(parameters->RegistersAddress + parameters->RegistersCount > MOCK_REGISTERS_COUNT)
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_ADDRESS_ERROR;

   memcpy(parameters->Data.AsPlain,
          &MockReadRegisters[parameters->RegistersAddress],
          parameters->RegistersCount * sizeof(ModbusSDeviceRegister));

   return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK;
}

ModbusSDeviceProtocolException WriteRegistersMock(ModbusSDeviceCommonHandle handle,
                                                  const ModbusSDeviceRegistersWriteParameters *parameters)
{
   if(parameters->RegistersAddress + parameters->RegistersCount > MOCK_REGISTERS_COUNT)
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_ADDRESS_ERROR;

   memcpy(&MockWriteRegisters[parameters->RegistersAddress],
          parameters->Data.AsPlain,
          parameters->RegistersCount * sizeof(ModbusSDeviceRegister));

   return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK;
}
