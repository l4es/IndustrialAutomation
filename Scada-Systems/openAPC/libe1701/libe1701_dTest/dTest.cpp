#include "liboapc.h"
#include <stdio.h>
#include <assert.h>

#include <string>

#pragma pack(push,1)

struct 
#ifndef ENV_WINDOWS
 __attribute__((__packed__))
#endif
command_data 
{
   char          id;
   unsigned char command;
   unsigned int  x,y,z;
};


struct 
#ifndef ENV_WINDOWS
 __attribute__((__packed__))
#endif
response_data 
{
   char          id;
   unsigned char command;
   unsigned int  buffer,state,digiIN;
};

#pragma pack(pop)

#define POLY 0x82f63b78

unsigned int crc32b(const char *buf,size_t len)
{
   int          k;
   unsigned int crc=0xFFFFFFFF;
//   size_t       len=strlen(buf);

   while (len--) 
   {
      crc^=*buf++;
      for (k=0; k<8; k++)
       crc=crc&1 ? (crc>>1)^POLY : crc>>1;
   }
   return ~crc;
}



int main(int argc, char* argv[])
{
   int         sock;
   std::string sendBuffer;
   int         sendLength=0;
#ifdef ENV_WINDOWS
   WSADATA   wsaData;

   WSAStartup(MAKEWORD(1, 1), &wsaData);
#endif

   printf("Trying to connect...\r\n");
   sock=oapc_tcp_connect_to("192.168.2.254",23);
   if (sock>0)
   {
      struct command_data  *data;
      struct response_data  response;
      char                  rawBuffer[100];
      int                   i;

      oapc_tcp_set_blocking(sock,0);
      oapc_tcp_recv(sock,rawBuffer,100,"\n",1000); // on connection telet server on E1701D sends an initialisation string which needs to be dropped

      data=(struct command_data*)rawBuffer;

      data->id='d';

      // *** example: mark some vector data *************************************************************************************
      //set laser/scanner parameters first
      data->command=0x0C;  // set laser mode
      data->x=0x48000000;  // CO2 laser
      data->y=0;
      data->z=0;  
      sendBuffer.append(rawBuffer,14);
      sendLength+=14;

      data->command=0x0E;  //frequency and period to be used during mark
      data->x=20000;
      data->y=1000;
      data->z=0;  
      sendBuffer.append(rawBuffer,14);
      sendLength+=14;

      data->command=0x0F;  // no frequency during jumps (very unusual for CO2 lasers)
      data->x=0;
      data->y=0;
      data->z=0;  
      sendBuffer.append(rawBuffer,14);
      sendLength+=14;

      data->command=0x04; // set jump and mark speeds
      data->x=50000<<10;  // jump speed
      data->y=10000<<10;  // mark speed
      data->z=0;                 
      sendBuffer.append(rawBuffer,14);
      sendLength+=14;

      data->command=0x06; // set delays
      data->x=0;   // jump delay
      data->y=0;   // mark delay
      data->z=0;   // in-polygon delay
      sendBuffer.append(rawBuffer,14);
      sendLength+=14;

      data->command=0x00; // jump to starting position
      data->x=10000<<10; // shift 16 bit value up to 26 bit
      data->y=10000<<10; // shift 16 bit value up to 26 bit
      data->z=0;
      sendBuffer.append(rawBuffer,14);
      sendLength+=14;

      for (i=0; i<4; i++) // mark rectangle several times
      {
         data->command=0x01;
         data->x=50000<<10; // shift 16 bit value up to 26 bit
         data->y=10000<<10; // shift 16 bit value up to 26 bit
         sendBuffer.append(rawBuffer,14);
         sendLength+=14;

         data->x=50000<<10; // shift 16 bit value up to 26 bit
         data->y=50000<<10; // shift 16 bit value up to 26 bit
         sendBuffer.append(rawBuffer,14);
         sendLength+=14;

         data->x=10000<<10; // shift 16 bit value up to 26 bit
         data->y=50000<<10; // shift 16 bit value up to 26 bit
         sendBuffer.append(rawBuffer,14);
         sendLength+=14;

         data->x=10000<<10; // shift 16 bit value up to 26 bit
         data->y=10000<<10; // shift 16 bit value up to 26 bit
         sendBuffer.append(rawBuffer,14);
         sendLength+=14;
      }

      data->command=0x02; //FLUSH data to start marking
      sendBuffer.append(rawBuffer,14); 
      sendLength+=14;

      // attention: when sending this in a loop application has to ensure the input buffer of E1701D does not overrun
      // -> check if response.buffer is greater than 10000
      oapc_tcp_send(sock,sendBuffer.c_str(),sendLength,100);
      sendLength=0;

      data->command=0x0A; // PING-command to get current state from card
      data->x=0;
      data->y=0;
      data->z=0;
      while (oapc_tcp_recv(sock,(char*)&response,14,NULL,500)>=14)
      {
         if ((response.id=='d') && (response.command==0xFF))
         {
            if ((response.state==0) &&            // marking operation has ended...
                (response.buffer==900000)) break; // ...and there are no more data to be processed left in buffer so loop and application can be left now

            printf("Response: buffer=%d, card state=0x%X\r\n",response.buffer,response.state);
         }
         oapc_thread_sleep(100); // do not stall the controller by sending too much data too fast
         oapc_tcp_send(sock,(const char*)data,14,100);
      }
      // *** end of example: mark some vector data *************************************************************************************

      // *** example: download new firmware to controller *************************************************************************************
      {
         char testString[]="HALaser Systems is specialised on integrated soft- and hardware solutions for laser marking industry and scientific researchers. HALaser Systems was founded as spin-off of OpenAPC Project Group to fully focus on all laser-marking related solutions of the OpenAPC software package. Our customers are companies of nearly every size worldwide and with nearly every background. Our products offer solutions for all kinds of businesses like machine , integrators, manufacturers, researchers and many others.";
         int  sent;

         data->command=0x45;
         data->x=strlen(testString); // WARNING! The current firmware file will be overwritten by this test string,
                                     // so for a real-world application here the binaries of a .fwi file have to be
                                     // downloaded instead!
         data->y=crc32b(testString,strlen(testString));
         data->z=0;
         oapc_tcp_send(sock,(const char*)data,14,100);

         // wait until request for sending has been accepted
         while (oapc_tcp_recv(sock,(char*)&response,14,NULL,500)>=14)
         {
            if ((response.id=='d') && (response.command==0xFF))
            {
               if (response.state!=0) // wait until operation is in progress...
               printf("Response: buffer=%d, card state=0x%X\r\n",response.buffer,response.state);
            }
            oapc_thread_sleep(100); // do not stall the controller by sending too much data too fast
            // do NOT send any pings in between here!
         }

         // send the data
         sent=oapc_tcp_send(sock,testString,strlen(testString),250);
         assert(sent==strlen(testString));
      }

      data->command=0x0A; // PING-command to get current state from card
      data->x=0;
      data->y=0;
      data->z=0;
      while (oapc_tcp_recv(sock,(char*)&response,14,NULL,500)>=14)
      {
         if ((response.id=='d') && (response.command==0xFF))
         {
            if ((response.state & 0x4000)==0) // wait until writing has finished by checking flag E1701_CSTATE_WRITING_DATA
            printf("Response: buffer=%d, card state=0x%X\r\n",response.buffer,response.state);
         }
         oapc_thread_sleep(100); // do not stall the controller by sending too much data too fast
         oapc_tcp_send(sock,(const char*)data,14,100);
      }
      if ((response.state & 0x8000)==0x8000)
       printf("ERROR: downloading of file has failed! Retrieve more detailed information via command \"cglog\"");
      // *** end of example: download new firmware to controller *************************************************************************************

      oapc_tcp_closesocket(sock);
   }
   else printf("Error connection to 192.168.2.254\r\n");
   return 0;
}
