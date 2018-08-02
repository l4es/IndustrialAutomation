#include <stdio.h>
#include <math.h>

#include "oapc_libio.h"
#include "liboapc.h"
#include "../libio_modbus_tcp_master/modbus_common.h"
#include "modbus_rtu_io.h"

#ifdef ENV_LINUX
#include <netinet/in.h>
#endif

#ifdef ENV_WINDOWSCE
 #include <winsock2.h>
 #include "time_ce.h"
#endif



static int add_crc(struct modrtu_adu *data,unsigned int size)
{
   unsigned int   crc,i,j,carry_flag,a;
   unsigned char *frame;

   frame=(unsigned char*)data;
   crc=0xFFFF;
   for (i=0; i<size; i++)
   {
      crc=crc^((unsigned int)frame[i]);
      for (j=0; j<8; j++)
      {
         a=crc;
         carry_flag=a & 0x0001;
         crc=crc>>1;
         if (carry_flag==1) crc=crc^0xA001;
      }
   }
   frame[size+1]=(unsigned char)((crc & 0xFF00)>>8);
   frame[size]  =(unsigned char)(crc & 0x00FF);

   return crc;
}



void writeLog(struct instData *data,const char *format,...)
{
   va_list  arglist;
   char     sText[1500];

   if (!data->FHandle) return;
   va_start(arglist,format);
   vsprintf(sText,format,arglist);
   va_end(arglist);
#ifdef ENV_WINDOWS
 #ifdef ENV_WINDOWSCE
   fprintf(data->FHandle,"%d\t%s\r\n",time_ce(NULL),sText);
 #else
   fprintf(data->FHandle,"%d\t%s\r\n",time(NULL),sText);
 #endif
#else
   fprintf(data->FHandle,"%d\t%s\n",(int)time(NULL),sText);
#endif
}



void swapByteorder(unsigned short *addr)
{
   unsigned char store;

   store=(unsigned char)(((*addr) & 0xFF00)>>8);
   *addr=(unsigned short)(((*addr)<<8) & 0xFF00);
   *addr|=store;
}




int writeMultiCoils(struct instData *data,unsigned char slaveAddr,unsigned short addrOffset,unsigned int value[],unsigned short wLength)
{
   struct modrtu_adu adu,recvAdu;
   char              recvBuffer[6];
   int               len,i;
	
   adu.slaveAddr=slaveAddr;
   adu.pdu.funcCode=MODCMD_WRITE_MULTI_COILS;
   adu.pdu.addrOffset=htons(addrOffset);
   if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==0)
    swapByteorder(&adu.pdu.addrOffset);

   len=32*wLength;
   if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_DATABYTEORDER)==0)
   {
       adu.pdu.data[0]=(unsigned char)(len & 0x00FF);
       adu.pdu.data[1]=(unsigned char)((len & 0xFF00)>>8);
   }
   else
   {
       adu.pdu.data[0]=(unsigned char)((len & 0xFF00)>>8);
       adu.pdu.data[1]=(unsigned char)(len & 0x00FF);
   }
   adu.pdu.data[2]=(unsigned char)(wLength*4);
	
   for (i=0; i<wLength; i++)
   {
      if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_DATABYTEORDER)==0)
      {
         adu.pdu.data[3+(i*4)]=(unsigned char)(value[i] & 0x000000FF);
         adu.pdu.data[4+(i*4)]=(unsigned char)((value[i] & 0x0000FF00)>>8);
         adu.pdu.data[5+(i*4)]=(unsigned char)((value[i] & 0x00FF0000)>>16);
         adu.pdu.data[6+(i*4)]=(unsigned char)((value[i] & 0xFF000000)>>24);
      }
      else
      {
         adu.pdu.data[6+(i*4)]=(unsigned char)(value[i] & 0x000000FF);
         adu.pdu.data[5+(i*4)]=(unsigned char)((value[i] & 0x0000FF00)>>8);
         adu.pdu.data[4+(i*4)]=(unsigned char)((value[i] & 0x00FF0000)>>16);
         adu.pdu.data[3+(i*4)]=(unsigned char)((value[i] & 0xFF000000)>>24);
      }
   }
   add_crc(&adu,7+(wLength*4));
	
   writeLog(data,"writeMultiCoils %d %d %d",slaveAddr,addrOffset,wLength);
   if (oapc_serial_send_data(data->m_fd,(char*)&adu,9+(wLength*4),1500)<9+(wLength*4))
   {
      writeLog(data,"ERR: sending of data failed");
	  return -1;
   }
   len=oapc_serial_recv_data(data->m_fd,(char*)&recvAdu,2,3500);
   if (len<2)
   {
      writeLog(data,"ERR: receiving of response failed (%d bytes)",len);
      return -1;
   }
   if (recvAdu.pdu.funcCode==adu.pdu.funcCode)
   {
      if (oapc_serial_recv_data(data->m_fd,recvBuffer,6,500)!=6)
  	  {
         writeLog(data,"ERR: receiving of response data failed");
         return -1;
  	  }
   }
   else
   {
      oapc_serial_recv_data(data->m_fd,recvBuffer,1,500); // error code
      writeLog(data,"ERR: response error code %d",recvBuffer[0]);
      return -1;
   }
   return 0;
}



int writeHoldReg(struct instData *data,unsigned char slaveAddr,unsigned short addrOffset,unsigned int value[],unsigned short wLength)
{
   struct modrtu_adu adu,recvAdu;
   char              recvBuffer[6];
   int               len,i;

   adu.slaveAddr=slaveAddr;
   adu.pdu.funcCode=MODCMD_WRITE_MULTI_HOLDREGS;
   adu.pdu.addrOffset=htons(addrOffset);
   if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==0)
    swapByteorder(&adu.pdu.addrOffset);

   if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_DATABYTEORDER)==0)
   {
      adu.pdu.data[0]=(unsigned char)(wLength & 0x00FF);
      adu.pdu.data[1]=(unsigned char)((wLength & 0xFF00)>>8);
   }
   else
   {
      adu.pdu.data[0]=(unsigned char)((wLength & 0xFF00)>>8);
      adu.pdu.data[1]=(unsigned char)(wLength & 0x00FF);
   }
   adu.pdu.data[2]=(unsigned char)wLength*2;

   for (i=0; i<wLength; i++)
   {
      if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_DATABYTEORDER)==0)
      {
         adu.pdu.data[3+(i*2)]=(unsigned char)((value[i] & 0x00FF));
         adu.pdu.data[4+(i*2)]=(unsigned char)((value[i] & 0xFF00)>>8);
      }
      else
      {
         adu.pdu.data[4+(i*2)]=(unsigned char)(value[i] & 0x00FF);
         adu.pdu.data[3+(i*2)]=(unsigned char)((value[i] & 0xFF00)>>8);
      }
   }

   add_crc(&adu,7+(wLength*2));

   writeLog(data,"writeHoldReg %d %d %d",slaveAddr,addrOffset,wLength);
   if (oapc_serial_send_data(data->m_fd,(char*)&adu,9+(wLength*2),1500)<9+(wLength*2))
   {
      writeLog(data,"ERR: sending of data failed");
      return -1;
   }
	len=oapc_serial_recv_data(data->m_fd,(char*)&recvAdu,2,3500);
   if (len<2)
   {
      writeLog(data,"ERR: receiving of response failed (%d bytes)",len);
      return -1;
   }
   if (recvAdu.pdu.funcCode==adu.pdu.funcCode)
   {
   	  if (oapc_serial_recv_data(data->m_fd,recvBuffer,6,500)!=6)
      {
         writeLog(data,"ERR: receiving of response data failed");
         return -1;
      }
   }
   else
   {
      oapc_serial_recv_data(data->m_fd,recvBuffer,1,500); // error code
      writeLog(data,"ERR: response error code %d",recvBuffer[0]);
      return -1;
   }
   return 0;
}



int readDisCoils(struct instData *data,unsigned char slaveAddr,unsigned short addrOffset,unsigned int *value,unsigned char funcCode,unsigned short rLength)
{
   struct modrtu_adu adu,recvAdu;
   unsigned char     recvBuffer[300];
   int               len,i;

   adu.slaveAddr=slaveAddr;
   adu.pdu.funcCode=funcCode;
   adu.pdu.addrOffset=htons(addrOffset);
   if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==0)
    swapByteorder(&adu.pdu.addrOffset);

   len=32*rLength;
   if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_DATABYTEORDER)==0)
   {
       adu.pdu.data[0]=(unsigned char)(len & 0x00FF);
       adu.pdu.data[1]=(unsigned char)((len & 0xFF00)>>8);
   }
   else
   {
       adu.pdu.data[0]=(unsigned char)((len & 0xFF00)>>8);
       adu.pdu.data[1]=(unsigned char)(len & 0x00FF);
   }
   add_crc(&adu,6);

   writeLog(data,"readDisCoils %d %d %d",slaveAddr,addrOffset,rLength);
   oapc_serial_recv_data(data->m_fd,(char*)recvBuffer,299,10); // flush garbage that may be sent by the device meanwhile
   if (oapc_serial_send_data(data->m_fd,(char*)&adu,8,1500)<8)
   {
      writeLog(data,"ERR: sending of data failed");
      return OAPC_ERROR_SEND_DATA;
   }
   len=oapc_serial_recv_data(data->m_fd,(char*)&recvAdu,2,3500);
   if (len<2)
   {
      writeLog(data,"ERR: receiving of response failed (%d bytes)",len);
      return OAPC_ERROR_RECV_DATA;
   }
   if (recvAdu.pdu.funcCode==adu.pdu.funcCode)
   {
      if (oapc_serial_recv_data(data->m_fd,(char*)recvBuffer,3+(rLength*4),500)!=3+(rLength*4))
      {
         writeLog(data,"ERR: receiving of response data failed");
         return OAPC_ERROR_RECV_DATA;
      }
      for (i=0; i<rLength; i++)
      {
         if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_DATABYTEORDER)==0)
          value[i]=(recvBuffer[1+(i*4)]|(recvBuffer[2+(i*4)]<<8)|(recvBuffer[3+(i*4)]<<16)|(recvBuffer[4+(i*4)]<<24));
         else
          value[i]=(recvBuffer[4+(i*4)]|(recvBuffer[3+(i*4)]<<8)|(recvBuffer[2+(i*4)]<<16)|(recvBuffer[1+(i*4)]<<24));
      }
   }
   else
   {
      oapc_serial_recv_data(data->m_fd,(char*)recvBuffer,1,500); // error code
      writeLog(data,"ERR: response error code %d",recvBuffer[0]);
      return OAPC_ERROR_RECV_DATA;
   }
   return OAPC_OK;
}



int readReg(struct instData *data,unsigned char slaveAddr,unsigned short addrOffset,unsigned int *value,unsigned char funcCode,unsigned short rLength)
{
   struct modrtu_adu adu,recvAdu;
   unsigned char     recvBuffer[300];
   int               len,i;

   adu.slaveAddr=slaveAddr;
   adu.pdu.funcCode=funcCode;
   adu.pdu.addrOffset=htons(addrOffset);
   if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==0)
    swapByteorder(&adu.pdu.addrOffset);

   if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_DATABYTEORDER)==0)
   {
      adu.pdu.data[0]=(unsigned char)(rLength & 0x00FF);
      adu.pdu.data[1]=(unsigned char)((rLength & 0xFF00)>>8);
   }
   else
   {
      adu.pdu.data[0]=(unsigned char)((rLength & 0xFF00)>>8);
      adu.pdu.data[1]=(unsigned char)(rLength & 0x00FF);
   }
   add_crc(&adu,6);

   writeLog(data,"readReg %d %d",slaveAddr,addrOffset);
   oapc_serial_recv_data(data->m_fd,(char*)recvBuffer,299,10); // flush garbage that may be sent by the device meanwhile
   if (oapc_serial_send_data(data->m_fd,(char*)&adu,8,1500)<8)
   {
      writeLog(data,"ERR: sending of data failed");
      return OAPC_ERROR_SEND_DATA;
   }
   len=oapc_serial_recv_data(data->m_fd,(char*)&recvAdu,2,3500);
   if (len<2)
   {
      writeLog(data,"ERR: receiving of response failed (%d bytes)",len);
      return OAPC_ERROR_RECV_DATA;
   }
   if (recvAdu.pdu.funcCode==adu.pdu.funcCode)
   {
      if (oapc_serial_recv_data(data->m_fd,(char*)recvBuffer,3+(rLength*2),500)!=3+(rLength*2))
      {
         writeLog(data,"ERR: receiving of response data failed");
         return OAPC_ERROR_RECV_DATA;
      }
      writeLog(data,"OK: received data bytes 0x%X 0x%X",recvBuffer[1],recvBuffer[2]);
      for (i=0; i<rLength; i++)
      {
         if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_DATABYTEORDER)==0)
          value[i]=(unsigned short)(recvBuffer[1+(i*2)]|(recvBuffer[2+(i*2)]<<8));
         else 
          value[i]=(unsigned short)(recvBuffer[2+(i*2)]|(recvBuffer[1+(i*2)]<<8));
      }
   }
   else
   {
      oapc_serial_recv_data(data->m_fd,(char*)recvBuffer,1,500); // error code
      writeLog(data,"ERR: response error code %d",recvBuffer[0]);
      return OAPC_ERROR_RECV_DATA;
   }
   return OAPC_OK;
}



int readFloatReg(struct instData *data,unsigned char slaveAddr,unsigned short addrOffset,double *value,unsigned char funcCode,unsigned short rLength)
{
   struct modrtu_adu adu,recvAdu;
   unsigned char     recvBuffer[300];
   int               len,i;

   adu.slaveAddr=slaveAddr;
   adu.pdu.funcCode=funcCode;
   adu.pdu.addrOffset=htons(addrOffset);
   if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_ADDRBYTEORDER)==0)
    swapByteorder(&adu.pdu.addrOffset);

   len=rLength*2;
   if ((data->config.flags & MODBUS_TCP_FLAG_INVERT_DATABYTEORDER)==0)
   {
      adu.pdu.data[0]=(unsigned char)(len & 0x00FF);
      adu.pdu.data[1]=(unsigned char)((len & 0xFF00)>>8);
   }
   else
   {
      adu.pdu.data[0]=(unsigned char)((len & 0xFF00)>>8);
      adu.pdu.data[1]=(unsigned char)(len & 0x00FF);
   }
   add_crc(&adu,6);

   writeLog(data,"readReg %d %d",slaveAddr,addrOffset);
   oapc_serial_recv_data(data->m_fd,(char*)recvBuffer,299,10); // flush garbage that may be sent by the device meanwhile
   if (oapc_serial_send_data(data->m_fd,(char*)&adu,8,1500)<8)
   {
      writeLog(data,"ERR: sending of data failed");
      return OAPC_ERROR_SEND_DATA;
   }
   len=oapc_serial_recv_data(data->m_fd,(char*)&recvAdu,2,3500);
   if (len<2)
   {
      writeLog(data,"ERR: receiving of response failed (%d bytes)",len);
      return OAPC_ERROR_RECV_DATA;
   }
   if (recvAdu.pdu.funcCode==adu.pdu.funcCode)
   {
      unsigned int  iVal,exp,mant;
      unsigned char sign;

      if (oapc_serial_recv_data(data->m_fd,(char*)recvBuffer,3+(rLength*4),500)!=3+(rLength*4))
      {
         writeLog(data,"ERR: receiving of response data failed");
         return OAPC_ERROR_RECV_DATA;
      }

      for (i=0; i<rLength; i++)
      {
         iVal=(recvBuffer[1+(i*4)]<<24)|(recvBuffer[2+(i*4)]<<16)|(recvBuffer[3+(i*4)]<<8)|(recvBuffer[4+(i*4)]);
         if (iVal& 0x80000000) sign=1;
         else sign=0;
         exp=(iVal & 0x7F800000)>>23;
         mant=(iVal & 0x007FFFFF);
         value[i]=pow((double)mant,(double)exp);
         if (sign) *value=*value*-1.0;
         writeLog(data,"int 0x%X converted to %f",iVal,value[i]);
      }
   }
   else
   {
      oapc_serial_recv_data(data->m_fd,(char*)recvBuffer,1,500); // error code
      writeLog(data,"ERR: response error code %d",recvBuffer[0]);
      return OAPC_ERROR_RECV_DATA;
   }
   return OAPC_OK;
}


int arraycmp(unsigned short s1[],unsigned int i2[],int len)
{
   int i;

   for (i=0; i<len; i++) if (s1[i]!=i2[i]) return i+1;
   return 0;
}


void arraycpy(unsigned short s1[],unsigned int i2[],int len)
{
   int i;

   for (i=0; i<len; i++) s1[i]=(unsigned short)i2[i];
}
