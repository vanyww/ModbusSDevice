#include "adu.h"
#include "../../../Inc/ModbusSDevice/TCP/tcp_defs.h"

#define __MODBUS_TCP_PROTOCOL_ID ((uint16_t)0x0000)

size_t ModbusTcpProcessAdu(__SDEVICE_HANDLE(Modbus) *handle, size_t requestSize)
{
   if(requestSize < sizeof(ModbusTcpAduData))
       return 0;

   ModbusTcpAduData *requestAdu = handle->Constant->ReceiveBuffer;

   if(requestAdu->ProtocolId != __MODBUS_TCP_PROTOCOL_ID)
       return 0;

   if(requestAdu->PacketSize != requestSize - offsetof(ModbusTcpAduData, SlaveAddress))
       return 0;

   uint8_t slaveAddress = requestAdu->SlaveAddress;
   size_t replyPduSize = ModbusProcessPdu(handle,
                                          &(ModbusCommonRequestProcessingData)
                                          {
                                             .PduSize = requestSize - offsetof(ModbusTcpAduData, Pdu),
                                             .RequestParameters = &(ModbusSDeviceTcpRequestData)
                                             {
                                                .SlaveAddress = slaveAddress
                                             }
                                          });

   ModbusTcpAduData *replyAdu = handle->Constant->TransmitBuffer;

   replyAdu->ProtocolId = __MODBUS_TCP_PROTOCOL_ID;
   replyAdu->TransactionId = requestAdu->TransactionId;
   replyAdu->SlaveAddress = slaveAddress;
   replyAdu->PacketSize = replyPduSize + offsetof(ModbusTcpAduData, Pdu) - offsetof(ModbusTcpAduData, SlaveAddress);

   return replyPduSize + offsetof(ModbusTcpAduData, Pdu);
}
