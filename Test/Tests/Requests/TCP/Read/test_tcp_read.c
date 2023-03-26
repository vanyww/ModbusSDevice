#include "test_tcp_read.h"
#include "../../../../Device/test_device.h"
#include "../../../../Device/Mock/Errors/errors.h"
#include "../../../../Device/Mock/Functions/mock_functions.h"

#include <memory.h>

bool TestTcpReadOneRequest(void)
{
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusTcp))))
         SDEVICE_HANDLE(ModbusTcp) *handle = CreateModbusTcpSDevice();

   MockReadRegisters[0] = 0x2211;

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x01, 0x03, 0x02, 0x11, 0x22 };
   ModbusSDeviceRequest mbapData = { .Data = request, .Size = MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE };
   ModbusSDeviceRequest requestData =
   {
      .Data = request + MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE,
      .Size = sizeof(request) - MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE
   };
   ModbusSDeviceResponse responseData = { .Data = (uint8_t[MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE]){ } };

   size_t packetSize;

   if(ModbusTcpSDeviceTryProcessMbapHeader(handle, &mbapData, &packetSize) != true)
      return false;

   if(ModbusTcpSDeviceTryProcessRequest(handle, &requestData, &responseData) != true)
      return false;

   if(AnyErrorDetected())
      return false;

   if(responseData.Size != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Data, responseData.Size) != 0)
      return false;

   return true;
}

bool TestTcpReadMultipleRequest(void)
{
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusTcp))))
         SDEVICE_HANDLE(ModbusTcp) *handle = CreateModbusTcpSDevice();

   MockReadRegisters[0] = 0x2211;
   MockReadRegisters[1] = 0x4433;

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x02 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x01, 0x03, 0x04, 0x11, 0x22, 0x33, 0x44 };
   ModbusSDeviceRequest mbapData = { .Data = request, .Size = MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE };
   ModbusSDeviceRequest requestData =
   {
      .Data = request + MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE,
      .Size = sizeof(request) - MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE
   };
   ModbusSDeviceResponse responseData = { .Data = (uint8_t[MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE]){ } };

   size_t packetSize;

   if(ModbusTcpSDeviceTryProcessMbapHeader(handle, &mbapData, &packetSize) != true)
      return false;

   if(ModbusTcpSDeviceTryProcessRequest(handle, &requestData, &responseData) != true)
      return false;

   if(AnyErrorDetected())
      return false;

   if(responseData.Size != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Data, responseData.Size) != 0)
      return false;

   return true;
}

bool TestTcpReadTooManyRequest(void)
{
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusTcp))))
         SDEVICE_HANDLE(ModbusTcp) *handle = CreateModbusTcpSDevice();

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0xFF };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x01, 0x83, 0x03 };
   ModbusSDeviceRequest mbapData = { .Data = request, .Size = MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE };
   ModbusSDeviceRequest requestData =
   {
      .Data = request + MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE,
      .Size = sizeof(request) - MODBUS_TCP_SDEVICE_MBAP_HEADER_SIZE
   };
   ModbusSDeviceResponse responseData = { .Data = (uint8_t[MODBUS_TCP_SDEVICE_MAX_MESSAGE_SIZE]){ } };

   size_t packetSize;

   if(ModbusTcpSDeviceTryProcessMbapHeader(handle, &mbapData, &packetSize) != true)
      return false;

   if(ModbusTcpSDeviceTryProcessRequest(handle, &requestData, &responseData) != true)
      return false;

   if(!AnyErrorDetected())
      return false;

   if(responseData.Size != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, responseData.Data, responseData.Size) != 0)
      return false;

   return true;
}
