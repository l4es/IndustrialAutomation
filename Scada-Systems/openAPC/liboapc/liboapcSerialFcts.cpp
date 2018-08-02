/********************************************************************************************

This program and source file is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

*********************************************************************************************/

#ifdef ENV_LINUX
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <arpa/inet.h>
 #include <termios.h>
 #include <unistd.h>
 #include <sys/ioctl.h>
 #include <linux/serial.h>
#endif

#ifdef ENV_QNX
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <arpa/inet.h>
 #include <termios.h>
 #include <unistd.h>
#endif

#ifdef ENV_WINDOWS
 #undef _UNICODE
 #include <windows.h>
#endif

#include "oapc_libio.h"
#include "liboapc.h"

//#define DEBUG_LOG // enabling this will create a debug log with all open/send/recv calls at C:\tmp\serial.log and slow down everything ;-)

#ifdef DEBUG_LOG
#include <stdio.h>

static void debug_log(const char *format,...)
{
   va_list arglist;
#ifdef DEBUG_CONSOLE
   va_start(arglist, format);
   vprintf(format,arglist);
   va_end(arglist);
   printf("\r\n");
#else
   FILE   *FHandle;

   FHandle = fopen("C:\\tmp\\serial.log", "at");
   if (!FHandle) return;

   fprintf(FHandle,"%d\t",static_cast<int>(time(NULL)));

   va_start(arglist,format);
   vfprintf(FHandle,format,arglist);
   va_end(arglist);
#ifdef ENV_WINDOWS
   fprintf(FHandle,"\r\n");
#else
   fprintf(FHandle,"\n");
#endif
   fclose(FHandle);
#endif //!DEBUG_CONSOLE
}

#endif


/**
Function for sending data over the serial interface
@param[in] fd the handler/file descriptor of the opened port that has to be used for data transmission
@param[in] msg the data to be send
@param[in] len the length of the data that have to be sent
@param[in] msecs the time that can be used to send the data, when this limit has exceeded and not all data have been
           sent transmission is stopped; for Windows operating systems this value has minimum 25 msecs
@return the number of bytes that could be send during the specified time
*/
#ifdef ENV_WINDOWS
OAPC_EXT_API int oapc_serial_send(HANDLE fd,const char *msg, int len,int msecs)
#else
OAPC_EXT_API int oapc_serial_send(int fd,const char *msg, int len,int msecs)
#endif
{
   int    slen=0;
   int    ctr=0;
#ifdef ENV_WINDOWS
   unsigned long   bytesWritten;

//   if (msecs<25) msecs=25;
#else
   long   bytesWritten;
#endif
   while ((slen<len) && (ctr<=msecs))
   {
#ifdef ENV_WINDOWS
      if (!WriteFile(fd,msg+slen,len-slen,&bytesWritten,NULL))
      {
         DWORD err=GetLastError();

         if ((err!=ERROR_IO_PENDING) &&
             (err!=ERROR_INVALID_USER_BUFFER) &&
             (err!=ERROR_NOT_ENOUGH_MEMORY)) return -slen;
      }
      if (bytesWritten>0) slen+=bytesWritten;
      else
      {
      	 if (ctr<msecs) Sleep(10); // wait only when allowed transmission time is smaller than current one, elsewhere allow to leave the loop immediately
         ctr+=10;
      }
#else
      bytesWritten=write(fd,msg+slen,len-slen);
      if (bytesWritten>0)
      {
      	 slen+=bytesWritten;
         fsync(fd);
      }
      else
      {
      	 if (ctr<msecs) usleep(5000); // wait only when allowed transmission time is smaller than current one, elsewhere allow to leave the loop immediately
         ctr+=5;
      }
#endif
   }
#ifdef DEBUG_LOG
   debug_log("send %d / %d",slen,len);
#endif
   return slen;
}


/**
Function for sending data over the serial interface
@param[in] fd the handler/file descriptor of the opened port that has to be used for data transmission
@param[in] msg the data to be send
@param[in] len the length of the data that have to be sent
@param[in] msecs the time that can be used to send the data, when this limit has exceeded and not all data have been
           sent transmission is stopped; for Windows operating systems this value has minimum 25 msecs
@return the number of bytes that could be send during the specified time
*/
#ifdef ENV_WINDOWS
OAPC_EXT_API int oapc_serial_send_data(HANDLE fd,const char *msg, int len,int msecs)
#else
OAPC_EXT_API int oapc_serial_send_data(int fd,const char *msg, int len,int msecs)
#endif
{
   return oapc_serial_send(fd,msg,len,msecs);
}


/**
Function to get data from the serial interface
@param[in] fd handle/file descriptor of the already opened serial port
@param[in] buffer pointer to a memory area where the stored data have to be written into
@param[in] toLoad the number of bytes that have to be read at maximum
@param[in] msecs the time that can be used to read, when this limit has exceeded the function returns
           also if less that "toLoad" data could be read
@return the number of bytes that could be read and have been written into the buffer
*/
#ifdef ENV_WINDOWS
OAPC_EXT_API int oapc_serial_recv_data(HANDLE fd,char *data,int toLoad,long msecs)
#else
OAPC_EXT_API int oapc_serial_recv_data(int fd,char *data,int toLoad,long msecs)
#endif
{
   int  ctr=0,loaded=0;
#ifndef ENV_WINDOWS
   long bytesRead=0;
#else
   unsigned long bytesRead;
#endif

   while (loaded<toLoad)
   {
#ifdef ENV_WINDOWS
      if (ReadFile(fd,data+loaded,toLoad-loaded,&bytesRead,NULL))
       loaded+=bytesRead;
      if (loaded==toLoad) return loaded;
      if (ctr<msecs) Sleep(10);
      ctr+=10;
#else
      bytesRead=read(fd,data+loaded,toLoad-loaded);
      if (bytesRead>0) loaded+=bytesRead;
      if (loaded==toLoad) return loaded;
      usleep(2000);
      ctr+=2;
#endif
      if (ctr>msecs)
      {
#ifdef DEBUG_LOG
         debug_log("recv %d / %d",loaded,toLoad);
#endif
         return loaded;
      }
   }
#ifdef DEBUG_LOG
   debug_log("recv %d / %d",loaded,toLoad);
#endif
   return loaded;
}



/**
Function to get data from the serial interface
@param[in] fd handle/file descriptor of the already opened serial port
@param[in] buffer pointer to a memory area where the stored data have to be written into
@param[in] toLoad the number of bytes that have to be read at maximum
@param[in] msecs the time that can be used to read, when this limit has exceeded the function returns
           also if less that "toLoad" data could be read
@return the number of bytes that could be read and have been written into the buffer
*/
#ifdef ENV_WINDOWS
OAPC_EXT_API int oapc_serial_recv(HANDLE fd,char *data,int len,const char *termStr,long msecs)
#else
OAPC_EXT_API int oapc_serial_recv(int fd,char *data,int len,const char *termStr,long msecs)
#endif
{
	int  readLen=0,rc,timeCnt=0;
	
	if (msecs<10) msecs=10;
	
	while ((readLen<len) && (timeCnt<msecs))
	{
      rc=oapc_serial_recv_data(fd,data+readLen,1,10);

      if (rc>0)
      {
         readLen+=rc;
         if (termStr)
         {
            if (readLen+1<len) data[readLen+1]=0;
            if (strstr(data,termStr)) return readLen;
         }
      }
      else
      {
         timeCnt+=10;
         oapc_thread_sleep(10);
      }
	}
return readLen;
}



/**
Opens a serial interface and configures the port parameters
@param[in] serialParam specifies the port name and the parameters of the serial interface;
           please note that the members of this structure are index values, not the
	   parameter values to configure with
@param[out] fd the handle/file descriptor of the opened interface
@return OAPC_OK when the port could be opened and configured successfully, an OAPC_ERROR_xxx
        error code otherwise
*/
#ifdef ENV_WINDOWS
static int set_nonblocking(const HANDLE *fd)
{
   COMMTIMEOUTS timeouts;

   timeouts.ReadIntervalTimeout        =MAXDWORD;
   timeouts.ReadTotalTimeoutMultiplier =0;
   timeouts.ReadTotalTimeoutConstant   =0;
   timeouts.WriteTotalTimeoutMultiplier=0;
   timeouts.WriteTotalTimeoutConstant  =250;

   if (!SetCommTimeouts(*fd, &timeouts))
   {
#ifdef DEBUG_LOG
      debug_log("SetCommTimeouts failed with %d", GetLastError());
#endif
      return OAPC_ERROR_DEVICE;
   }
   return OAPC_OK;
}


static int serial_port_open(const struct serial_params *serialParams,HANDLE *fd)
#else
OAPC_EXT_API int oapc_serial_port_open(struct serial_params *serialParams,int *fd)
#endif
{
#ifdef ENV_WINDOWS
   DCB          ComSettings;

   memset(&ComSettings,0,sizeof(ComSettings));
   ComSettings.DCBlength=sizeof(DCB);

#ifdef DEBUG_LOG
   debug_log("CreateFile %s",serialParams->port);
#endif

#ifdef ENV_WINDOWSCE
   wchar_t widePort[100];

   if (oapc_util_to_unicode(serialParams->port,widePort,99))
    *fd=CreateFile(widePort,GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,0,0);
   else return OAPC_ERROR;
#else
   *fd=CreateFile(serialParams->port,GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,0,0);
#endif
   if ((!*fd) || (*fd==INVALID_HANDLE_VALUE))
   {
#ifdef DEBUG_LOG
      debug_log("CreateFile failed with %d", GetLastError());
#endif
      *fd=NULL;
      return OAPC_ERROR_DEVICE;
   }
   if (!GetCommState(*fd,&ComSettings))
   {
#ifdef DEBUG_LOG
      debug_log("GetCommState failed with %d",GetLastError());
#endif
      CloseHandle(*fd);
      *fd=NULL;
      return OAPC_ERROR_DEVICE;
   }

   ComSettings.DCBlength=sizeof(DCB);
   ComSettings.fBinary=TRUE;
   ComSettings.fDtrControl=DTR_CONTROL_DISABLE;
   ComSettings.fRtsControl=RTS_CONTROL_DISABLE;
   ComSettings.fAbortOnError=FALSE;
   ComSettings.fDsrSensitivity=FALSE;
   ComSettings.fTXContinueOnXoff=FALSE;
   ComSettings.fOutX=FALSE;
   ComSettings.fInX=FALSE;
   ComSettings.fRtsControl=RTS_CONTROL_DISABLE;
   ComSettings.fOutxCtsFlow=FALSE;
   ComSettings.fErrorChar=FALSE;
   ComSettings.wReserved=0;
   ComSettings.XonLim = 0;
   ComSettings.XoffLim=0;
   ComSettings.XonChar=0;
   ComSettings.XoffChar=0;
   ComSettings.ErrorChar=0;
   ComSettings.EofChar=0;
   ComSettings.EvtChar=0;

   switch (serialParams->brate)
   {
      case 1:
         ComSettings.BaudRate=CBR_110;
         break;
      case 2:
         ComSettings.BaudRate=CBR_300;
         break;
      case 3:
         ComSettings.BaudRate=CBR_600;
         break;
      case 4:
         ComSettings.BaudRate=CBR_2400;
         break;
      case 5:
         ComSettings.BaudRate=CBR_4800;
         break;
      case 6:
         ComSettings.BaudRate=CBR_9600;
         break;
      case 7:
         ComSettings.BaudRate=CBR_19200;
         break;
      case 8:
         ComSettings.BaudRate=CBR_38400;
         break;
      case 9:
         ComSettings.BaudRate=CBR_57600;
         break;
      case 10:
         ComSettings.BaudRate=CBR_115200;
         break;
      case 11:
         ComSettings.BaudRate=230400;
         break;
      case 12:
          ComSettings.BaudRate=CBR_256000;
          break;
      case 13:
          ComSettings.BaudRate=460800;
          break;
      case 14:
          ComSettings.BaudRate=512000;
          break;
      case 15:
          ComSettings.BaudRate=921600;
          break;
      case 16:
          ComSettings.BaudRate=1024000;
          break;
      default:
         ComSettings.BaudRate=0; // something invalid
         break;
   }
   switch (serialParams->databits)
   {
      case 1:
         ComSettings.ByteSize=5;
         break;
      case 2:
         ComSettings.ByteSize=6;
         break;
      case 3:
         ComSettings.ByteSize=7;
         break;
      case 4:
      default:
         ComSettings.ByteSize=8;
         break;
   }
   switch (serialParams->parity)
   {
      default:
      case 1:
         ComSettings.Parity=NOPARITY;
         ComSettings.fParity=0;
         break;
      case 2:
         ComSettings.Parity=EVENPARITY;
         ComSettings.fParity=1;
         break;
      case 3:
         ComSettings.Parity=ODDPARITY;
         ComSettings.fParity=1;
         break;
   }
   switch (serialParams->stopbits)
   {
      default:
      case 1:
          ComSettings.StopBits = ONESTOPBIT;
         break;
      case 2:
         ComSettings.StopBits=ONE5STOPBITS;
         break;
      case 3:
         ComSettings.StopBits=TWOSTOPBITS;
         break;
   }
   switch (serialParams->flowcontrol)
   {
      case 1:
         ComSettings.fOutX=FALSE;
         ComSettings.fInX=FALSE;
         break;
      case 2:
         ComSettings.fOutX=TRUE;
         ComSettings.fInX=TRUE;
         break;
      case 3:
         ComSettings.fRtsControl=RTS_CONTROL_TOGGLE;
         ComSettings.fOutxCtsFlow=TRUE;
         break;
      default:
         break;
   }

   if (!SetCommState(*fd,&ComSettings))
   {
#ifdef DEBUG_LOG
      debug_log("SetCommState failed with %d", GetLastError());
#endif
      CloseHandle(*fd);
      *fd=NULL;
      return OAPC_ERROR_DEVICE;
   }

   int ret=set_nonblocking(fd);
   if (ret != OAPC_OK) return ret;

#else
 #ifdef ENV_LINUX
   struct termios       options;
   struct serial_struct sStruct;

   *fd= open(serialParams->port, O_RDWR|O_NOCTTY);// | O_NDELAY);
   if (*fd==-1) return OAPC_ERROR_DEVICE;
   fcntl(*fd, F_SETFL,FNDELAY);

   tcgetattr(*fd, &options);

   //Enable the receiver and set local mode
   options.c_cflag |= (CLOCAL | CREAD);

   options.c_cflag &= ~CSIZE; // Mask the character size bits
   switch (serialParams->databits)
   {
      case 1:
         options.c_cflag |= CS5;
         break;
      case 2:
         options.c_cflag |= CS6;
         break;
      case 3:
         options.c_cflag |= CS7;
         break;
      case 4:
         options.c_cflag |= CS8;
         break;
      default:
         break;
   }
   
   switch (serialParams->parity)
   {
      case 1:
         options.c_cflag &= ~(PARENB|PARODD);
//         options.c_iflag &= ~(INPCK | ISTRIP);
         break;
      case 2:
         options.c_cflag |= PARENB;
         options.c_cflag &= ~PARODD;
//         options.c_iflag |= (INPCK | ISTRIP);
         break;
      case 3:
         options.c_cflag |= PARENB;
         options.c_cflag |= PARODD;
//         options.c_iflag |= (INPCK | ISTRIP);
         break;
      default:
         break;
   }

   switch (serialParams->stopbits)
   {
      case 1:
         options.c_cflag&=~CSTOPB;
         break;
      case 2:
         break;
      case 3:
         options.c_cflag|=CSTOPB;
         break;
      default:
         options.c_cflag&=~CSTOPB;
         break;
   }

   options.c_lflag=0;
   options.c_iflag=0;
   options.c_oflag=0;

   switch (serialParams->flowcontrol)
   {
      case 0:
         options.c_iflag &= ~(IXON | IXOFF | IXANY);
         options.c_cflag &= ~CRTSCTS;
         break;
      case 1:
         options.c_iflag |= (IXON | IXOFF | IXANY);
         options.c_cflag &= ~CRTSCTS;
         break;
      case 2:
         options.c_iflag &= ~(IXON | IXOFF | IXANY);
         options.c_cflag|=CRTSCTS;
         break;
      default:
         break;
   }

/*   options.c_iflag &= ~(INPCK | ISTRIP);
   options.c_iflag |=IGNPAR;

   options.c_lflag&=~(ICANON | ECHO | ECHOE |ECHOK|ISIG|IEXTEN|ECHONL);
   options.c_iflag&=~(IGNCR|IUTF8|ICRNL|INLCR|IUCLC|IMAXBEL);
   options.c_oflag&=~(ONLCR|OCRNL|OLCUC);*/


   ioctl(*fd, TIOCGSERIAL, &sStruct);
   sStruct.flags &= ~ASYNC_SPD_MASK;
   ioctl(*fd, TIOCSSERIAL, &sStruct);
   switch (serialParams->brate)
   {
      case 1:
         cfsetispeed(&options, B110);
         cfsetospeed(&options, B110);
         break;
      case 2:
         cfsetispeed(&options, B300);
         cfsetospeed(&options, B300);
         break;
      case 3:
         cfsetispeed(&options, B1200);
         cfsetospeed(&options, B1200);
         break;
      case 4:
         cfsetispeed(&options, B2400);
         cfsetospeed(&options, B2400);
         break;
      case 5:
         cfsetispeed(&options, B4800);
         cfsetospeed(&options, B4800);
         break;
      case 6:
         cfsetispeed(&options, B9600);
         cfsetospeed(&options, B9600);
         break;
      case 7:
         cfsetispeed(&options, B19200);
         cfsetospeed(&options, B19200);
         break;
      case 8:
         cfsetispeed(&options, B38400);
         cfsetospeed(&options, B38400);
         break;
      case 9:
         cfsetispeed(&options, B57600);
         cfsetospeed(&options, B57600);
         break;
      case 10:
         cfsetispeed(&options, B115200);
         cfsetospeed(&options, B115200);
         break;
      case 11:
         cfsetispeed(&options, B230400);
         cfsetospeed(&options, B230400);
         break;
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
          {
              int                  speed;

              switch (serialParams->brate)
              {
                 case 12:
                    speed=256000;
                    break;
                 case 13:
                    speed=460800;
                    break;
                 case 14:
                    speed=512000;
                    break;
                 case 15:
                    speed=921600;
                    break;
                 case 16:
                    speed=1024000;
                    break;
                 default:
                     speed=9600;
                     break;
              }

              ioctl(*fd, TIOCGSERIAL, &sStruct);
              sStruct.flags = (sStruct.flags & ~ASYNC_SPD_MASK) | ASYNC_SPD_CUST;
              sStruct.custom_divisor = (sStruct.baud_base + (speed / 2)) / speed;
              ioctl(*fd, TIOCSSERIAL, &sStruct);

              cfsetispeed(&options, B38400);
              cfsetospeed(&options, B38400);
              break;
          }
      default:
         cfsetispeed(&options, 0);
         cfsetospeed(&options, 0);
         break;
   }

   if (tcsetattr(*fd, TCSANOW, &options)!=0) return OAPC_ERROR_DEVICE;
 #else
  #ifdef ENV_QNX
   struct termios options;
   int            flags;

   *fd=open(serialParams->port, O_RDWR|O_NOCTTY);
   if (*fd== -1) return OAPC_ERROR_DEVICE;
   flags=fcntl(*fd,F_GETFL,0);
   flags|=O_NONBLOCK;
   fcntl(*fd, F_SETFL,flags);

   tcgetattr(*fd, &options);

   //Enable the receiver and set local mode
   options.c_cflag |= (CLOCAL | CREAD);

   options.c_cflag &= ~CSIZE; // Mask the character size bits
   switch (serialParams->databits)
   {
      case 1:
         options.c_cflag |= CS5;
         break;
      case 2:
         options.c_cflag |= CS6;
         break;
      case 3:
         options.c_cflag |= CS7;
         break;
      case 4:
         options.c_cflag |= CS8;
         break;
      default:
         break;
   }
   
   switch (serialParams->parity)
   {
      case 1:
         options.c_cflag &= ~(PARENB|PARODD);
//         options.c_iflag &= ~(INPCK | ISTRIP);
         break;
      case 2:
         options.c_cflag |= PARENB;
         options.c_cflag &= ~PARODD;
//         options.c_iflag |= (INPCK | ISTRIP);
         break;
      case 3:
         options.c_cflag |= PARENB;
         options.c_cflag |= PARODD;
//         options.c_iflag |= (INPCK | ISTRIP);
         break;
      default:
         break;
   }
   options.c_iflag &= ~(INPCK | ISTRIP);
   options.c_iflag |=IGNPAR;

   switch (serialParams->stopbits)
   {
      case 1:
         options.c_cflag&=~CSTOPB;
         break;
      case 2:
         break;
      case 3:
         options.c_cflag|=CSTOPB;
         break;
      default:
         options.c_cflag&=~CSTOPB;
         break;
   }

   switch (serialParams->flowcontrol)
   {
      case 1:
         options.c_iflag &= ~(IXON | IXOFF | IXANY);
         options.c_cflag &= ~(IHFLOW|OHFLOW);
         break;
      case 2:
         options.c_iflag |= (IXON | IXOFF | IXANY);
         options.c_cflag &= ~(IHFLOW|OHFLOW);
         break;
      case 3:
         options.c_iflag &= ~(IXON | IXOFF | IXANY);
         options.c_cflag|=(IHFLOW|OHFLOW);
         break;
      default:
         break;
   }

   options.c_lflag &= ~(ICANON | ECHO | ECHOE |ECHOK|ISIG|IEXTEN|ECHONL);
   options.c_iflag&=~(IGNCR);
   options.c_oflag&=~(ONLCR|OCRNL);

   switch (serialParams->brate)
   {
      case 1:
         cfsetispeed(&options, B110);
         cfsetospeed(&options, B110);
         break;
      case 2:
         cfsetispeed(&options, B300);
         cfsetospeed(&options, B300);
         break;
      case 3:
         cfsetispeed(&options, B1200);
         cfsetospeed(&options, B1200);
         break;
      case 4:
         cfsetispeed(&options, B2400);
         cfsetospeed(&options, B2400);
         break;
      case 5:
         cfsetispeed(&options, B4800);
         cfsetospeed(&options, B4800);
         break;
      case 6:
         cfsetispeed(&options, B9600);
         cfsetospeed(&options, B9600);
         break;
      case 7:
         cfsetispeed(&options, B19200);
         cfsetospeed(&options, B19200);
         break;
      case 8:
         cfsetispeed(&options, B38400);
         cfsetospeed(&options, B38400);
         break;
      case 9:
         cfsetispeed(&options, B57600);
         cfsetospeed(&options, B57600);
         break;
      case 10:
         cfsetispeed(&options, B115200);
         cfsetospeed(&options, B115200);
         break;
      case 11:
          cfsetispeed(&options, 230400);
          cfsetospeed(&options, 230400);
          break;
      case 12:
          cfsetispeed(&options, 250000);
          cfsetospeed(&options, 250000);
          break;
      case 13:
          cfsetispeed(&options, 460800);
          cfsetospeed(&options, 460800);
          break;
      case 14:
          cfsetispeed(&options, 500000);
          cfsetospeed(&options, 500000);
          break;
      case 15:
          cfsetispeed(&options, 921600);
          cfsetospeed(&options, 921600);
          break;
      case 16:
          cfsetispeed(&options, 1000000);
          cfsetospeed(&options, 1000000);
          break;
      default:
         cfsetispeed(&options, 0);
         cfsetospeed(&options, 0);
         break;
   }

   if (tcsetattr(*fd, TCSANOW, &options)!=0) return OAPC_ERROR_DEVICE;
   flock(*fd,LOCK_EX);
  #else
   #error Not implemented!
  #endif
 #endif
#endif
   return OAPC_OK;
}

#ifdef ENV_WINDOWS
OAPC_EXT_API int oapc_serial_port_open(struct serial_params *serialParams, HANDLE *fd)
{
   int i,ret;

   // this is a workaround for a Windows10 bug where calls to SetCommState() randomly fail without any reason
   for (i=0; i<9; i++)
   {
      ret=serial_port_open(serialParams,fd);
      if (ret==OAPC_OK) return ret;
   }
   return ret;
}

OAPC_EXT_API int oapc_serial_usb_port_open(const char *portname, HANDLE *fd)
{
   char usePort[MAX_TTY_SIZE+10];
   int  ret;
#ifdef DEBUG_LOG
   debug_log("USB direct CreateFile %s",portname);
#endif

   *fd=CreateFile(portname,GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,0,0);
   if ((!*fd) || (*fd==INVALID_HANDLE_VALUE))
   {
      _snprintf(usePort,MAX_TTY_SIZE,"\\\\.\\%s",portname);
#ifdef DEBUG_LOG
      debug_log("USB mapped CreateFile %s",usePort);
#endif

      *fd=CreateFile(usePort,GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,0,0);
      if ((!*fd) || (*fd==INVALID_HANDLE_VALUE))
      {
         struct serial_params sParams={"",16,4,1,1,0};
#ifdef DEBUG_LOG
         debug_log("CreateFile failed with %d", GetLastError());
#endif
         *fd=NULL;
      
         strncpy(sParams.port,portname,MAX_TTY_SIZE);
         if (oapc_serial_port_open(&sParams,fd)!=OAPC_OK)
         {
            strncpy(sParams.port,usePort,MAX_TTY_SIZE);
            return oapc_serial_port_open(&sParams,fd);
         }
      }
      ret=set_nonblocking(fd);
      if (ret!=OAPC_OK) return ret;
   }
   ret=set_nonblocking(fd);
   if (ret!=OAPC_OK) return ret;
   return OAPC_OK;
}

#else // ENV_WINDOWS

OAPC_EXT_API int oapc_serial_usb_port_open(const char *portname, int *fd)
{
   struct serial_params sParams={"",16,4,1,1,0};

   *fd= open(portname, O_RDWR|O_NOCTTY);// | O_NDELAY);
   if (*fd==-1) return OAPC_ERROR_DEVICE;

   strncpy(sParams.port,portname,MAX_TTY_SIZE);
   return oapc_serial_port_open(&sParams,fd);
}

#endif // !ENV_WINDOWS


/**
Closes a serial port and resets its handle/file descriptor so that it can't be used
any longer
@param[in/out] fd handle/file descriptor of the port to be closed
*/
#ifdef ENV_WINDOWS
OAPC_EXT_API void oapc_serial_port_close(HANDLE *fd)
#else
OAPC_EXT_API void oapc_serial_port_close(int *fd)
#endif
{
#ifdef ENV_WINDOWS
   CloseHandle(*fd);
   fd=NULL;
#else
   close(*fd);
   *fd=0;
#endif
}
