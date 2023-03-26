#pragma once

#include "ModbusSDevice/public_rtu.h"

SDEVICE_RUNTIME_DATA_FORWARD_DECLARATION(ModbusRtu);

SDEVICE_RUNTIME_DATA_DECLARATION(ModbusRtu)
{
   SDEVICE_PROPERTY_TYPE(ModbusRtu, SlaveAddress) SlaveAddress;
};

SDEVICE_HANDLE_DECLARATION(ModbusRtu);
