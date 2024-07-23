#include "test_udp_read.h"
#include "../../../../Mock/Errors/errors.h"
#include "../../../../Mock/SDevice/test_device.h"
#include "../../../../Mock/SDevice/Bindings/bindings.h"

#include <memory.h>

bool TestUdpReadOneRequest(void)
{
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusUdp))))
         SDEVICE_HANDLE(ModbusUdp) *handle = CreateModbusUdpSDevice();

   const uint8_t btuAddress[] = { 1, 2, 3, 4, 5, 6 };
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusUdp, BtuAddress)(handle, btuAddress);

   MockReadRegisters[0] = 0x2211;

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x01, 0x03, 0x02, 0x11, 0x22 };
   uint8_t replyBuffer[MODBUS_UDP_SDEVICE_MAX_MESSAGE_SIZE];
   size_t replySize;

   ModbusUdpSDeviceInput input = { request, sizeof(request) };
   ModbusUdpSDeviceOutput output = { replyBuffer, &replySize };

   if(ModbusUdpSDeviceTryProcessRequest(handle, input, output) != true)
      return false;

   if(WAS_ANY_ERROR_THROWN())
      return false;

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, replyBuffer, replySize) != 0)
      return false;

   return true;
}

bool TestUdpReadMultipleRequest(void)
{
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusUdp))))
         SDEVICE_HANDLE(ModbusUdp) *handle = CreateModbusUdpSDevice();

   const uint8_t btuAddress[] = { 1, 2, 3, 4, 5, 6 };
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusUdp, BtuAddress)(handle, btuAddress);

   MockReadRegisters[0] = 0x2211;
   MockReadRegisters[1] = 0x4433;

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x02 };
   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x01, 0x03, 0x04, 0x11, 0x22, 0x33, 0x44 };
   uint8_t replyBuffer[MODBUS_UDP_SDEVICE_MAX_MESSAGE_SIZE];
   size_t replySize;

   ModbusUdpSDeviceInput input = { request, sizeof(request) };
   ModbusUdpSDeviceOutput output = { replyBuffer, &replySize };

   if(ModbusUdpSDeviceTryProcessRequest(handle, input, output) != true)
      return false;

   if(WAS_ANY_ERROR_THROWN())
      return false;

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, replyBuffer, replySize) != 0)
      return false;

   return true;
}

bool TestUdpReadOneBtuRequest(void)
{
   __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(ModbusUdp))))
         SDEVICE_HANDLE(ModbusUdp) *handle = CreateModbusUdpSDevice();

   const uint8_t btuAddress[] = { 1, 2, 3, 4, 5, 6 };
   SDEVICE_SET_SIMPLE_PROPERTY(ModbusUdp, BtuAddress)(handle, btuAddress);

   MockReadRegisters[0] = 0x2211;

   const uint8_t request[] = { 0x00, 0x01, 0x00, 0x74, 0x00, 0x0C, 0x01,
                               1,    2,    3,    4,    5,    6,
                               0x03, 0x00, 0x00, 0x00, 0x01 };

   const uint8_t expectedReply[] = { 0x00, 0x01, 0x00, 0x74, 0x00, 0x0B, 0x01,
                                     1,    2,    3,    4,    5,    6,
                                     0x03, 0x02, 0x11, 0x22 };

   uint8_t replyBuffer[MODBUS_UDP_SDEVICE_MAX_MESSAGE_SIZE];
   size_t replySize;

   ModbusUdpSDeviceInput input = { request, sizeof(request) };
   ModbusUdpSDeviceOutput output = { replyBuffer, &replySize };

   if(ModbusUdpSDeviceTryProcessRequest(handle, input, output) != true)
      return false;

   if(WAS_ANY_ERROR_THROWN())
      return false;

   if(replySize != sizeof(expectedReply))
      return false;

   if(memcmp(expectedReply, replyBuffer, replySize) != 0)
      return false;

   return true;
}
