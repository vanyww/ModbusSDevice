#include "io.h"

#include <memory.h>

uint16_t MockReadRegisters[MOCK_REGISTERS_COUNT];
uint16_t MockWriteRegisters[MOCK_REGISTERS_COUNT];

ModbusSDeviceProtocolException MockReadOperation(void                                       *handle,
                                                 const ModbusSDeviceReadOperationParameters *parameters,
                                                 const void                                 *callContext)
{
   if(parameters->RegistersAddress + parameters->RegistersCount > MOCK_REGISTERS_COUNT)
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_ADDRESS;

   memcpy(parameters->RegistersData,
          &MockReadRegisters[parameters->RegistersAddress],
          parameters->RegistersCount * MODBUS_SDEVICE_REGISTER_SIZE);

   return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK;
}

ModbusSDeviceProtocolException MockWriteOperation(void                                        *handle,
                                                  const ModbusSDeviceWriteOperationParameters *parameters,
                                                  const void                                  *callContext)
{
   if(parameters->RegistersAddress + parameters->RegistersCount > MOCK_REGISTERS_COUNT)
      return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_ILLEGAL_DATA_ADDRESS;

   memcpy(&MockWriteRegisters[parameters->RegistersAddress],
          parameters->RegistersData,
          parameters->RegistersCount * MODBUS_SDEVICE_REGISTER_SIZE);

   return MODBUS_SDEVICE_PROTOCOL_EXCEPTION_OK;
}
