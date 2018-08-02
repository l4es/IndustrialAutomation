#ifndef MODBUS_RTU_IO_H
#define MODBUS_RTU_IO_H

#include <stdarg.h>

#define MAX_XML_SIZE    15000
#define MAX_CMDVAL_SIZE   250
#define MAX_SENDBUF_SIZE  100
#define MAX_FILENAME_SIZE 200


#pragma pack(1)

struct modrtu_adu
{
   unsigned char  slaveAddr; // 1..247
   struct mod_pdu pdu;
   // CRC is added using struct mod_pdu
};
struct mod_config
{
   unsigned char  mode;
   unsigned char  slaveAddr;
   unsigned short addrOffset;
};

struct mod_data
{
   // data storage for IOs of plug-in
   unsigned int   *wData;
   unsigned int   *rData;
   double         *frData;
   // data storage of data read from MODBUS to be compared with last read data
   unsigned int   *lData;
   unsigned short *sData;
   double         *fData;
   //
   unsigned char   writeNewData,readNewData;
   unsigned short  wLengthCtr,rLengthCtr;
   bool            noInitialCall;
};

#pragma pack(8)

struct libio_config
{
   unsigned short       version,length;
   struct serial_params serialParams;
   struct mod_config    modConfig[MAX_NUM_IOS];
   unsigned short       pollDelay;
   unsigned int         flags;
   char                 logfile[MAX_FILENAME_SIZE+4];
   // version 5
   unsigned short       rwLength[MAX_NUM_IOS]; // for backwards compatibility reasons this can't be part of struct mod_config
};

#define MODBUS_TCP_FLAG_LOG_COMMUNICATION    0x0001
#define MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER 0x0002
#define MODBUS_TCP_FLAG_INVERT_DATABYTEORDER 0x0008

struct instData
{
   struct libio_config config;
   struct mod_data     modData[MAX_NUM_IOS];
   struct mod_config   useConfig[2]; // for MODBUS RTU Addr only
#ifdef ENV_WINDOWS
   HANDLE              m_fd;
#else
   int                 m_fd;
#endif
   int                 m_callbackID;
   FILE               *FHandle;
};


extern int  writeMultiCoils(struct instData *data,unsigned char slaveAddr,unsigned short addrOffset,unsigned int value[],unsigned short wLength);
extern int  writeHoldReg(struct instData *data,unsigned char slaveAddr,unsigned short addrOffset,unsigned int value[],unsigned short wLength);
extern int  readDisCoils(struct instData *data,unsigned char slaveAddr,unsigned short addrOffset,unsigned int *value,unsigned char funcCode,unsigned short rLength);
extern int  readReg(struct instData *data,unsigned char slaveAddr,unsigned short addrOffset,unsigned int *value,unsigned char funcCode,unsigned short rLength);
extern int  readFloatReg(struct instData *data,unsigned char slaveAddr,unsigned short addrOffset,double *value,unsigned char funcCode,unsigned short rLength);

extern void writeLog(struct instData *data,const char *format,...);
extern void swapByteorder(unsigned short *addr);
extern int  arraycmp(unsigned short s1[],unsigned int i2[],int len);
extern void arraycpy(unsigned short s1[],unsigned int i2[],int len);

#endif
