#ifndef MODBUS_TCP_COMMON_H
#define MODBUS_TCP_COMMON_H

#define MAX_IP_SIZE 128
#define MAX_FILENAME_SIZE 200

#pragma pack(1)

#define MODBUS_TCP_FLAG_LOG_COMMUNICATION    0x0001
#define MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER 0x0002
#define MODBUS_TCP_FLAG_1x_COMPATIBILITY     0x0004
#define MODBUS_TCP_FLAG_INVERT_DATABYTEORDER 0x0008

struct modtcp_adu_head
{
   unsigned short transId,protId,length;
};

struct modtcp_adu
{
   struct modtcp_adu_head head;
   unsigned char          unitId;
   struct mod_pdu         pdu;
};

struct mod_config
{
   unsigned char  mode;
   unsigned short addrOffset;
};

struct mod_data
{
   unsigned int   wData;
   unsigned int   rData;
   double         frData;
   unsigned char  writeNewData;
};

#pragma pack(8)

#endif
