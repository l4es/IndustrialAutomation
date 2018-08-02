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

// taken from http://mockmoon-cybernetics.ch/computer/linux/programming/parport.html

#ifdef ENV_LINUX
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <arpa/inet.h>
 #include <termios.h>
 #include <unistd.h>
 #include <sys/ioctl.h>
 #include <linux/ppdev.h>
 #include <linux/lp.h>
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



/**
Function for sending data over the parallel interface
@param[in] fd the handler/file descriptor of the opened port that has to be used for data transmission
@param[in] msg the data to be send
@param[in] len the length of the data that have to be sent
@param[in] msecs the time that can be used to send the data, when this limit has exceeded and not all data have been
           sent transmission is stopped; for Windows operating systems this value has minimum 25 msecs
@return the number of bytes that could be send during the specified time
*/
#ifdef ENV_WINDOWS
OAPC_EXT_API int oapc_parallel_send(HANDLE fd,const char *msg, int len,int msecs)
#else
OAPC_EXT_API int oapc_parallel_send(int fd,const char *msg, int len,int msecs)
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
      WriteFile(fd,msg+slen,len-slen,&bytesWritten,NULL);
      if (bytesWritten>0) slen+=bytesWritten;
      else
      {
      	 if (ctr<msecs) Sleep(10); // wait only when allowed transmission time is smaller than current one, elsewhere allow to leave the loop immediately
         ctr+=10;
      }
#else
      ioctl(fd,PPWDATA,msg[slen]);
      slen++;
#endif
   }
   return slen;
}



/**
Opens a parallel interface and configures the port parameters
@param[in] portname name of the interface to be opened
@param[out] fd the handle/file descriptor of the opened interface
@return OAPC_OK when the port could be opened and configured successfully, an OAPC_ERROR_xxx
        error code otherwise
*/
#ifdef ENV_WINDOWS
OAPC_EXT_API int oapc_parallel_port_open(const char *portname,HANDLE *fd)
#else
OAPC_EXT_API int oapc_parallel_port_open(const char *portname,int *fd)
#endif
{
#ifdef ENV_WINDOWS
   *fd=CreateFile(portname,GENERIC_WRITE,0,0,OPEN_EXISTING,0,0);
   if ((!*fd) || (*fd==INVALID_HANDLE_VALUE))
   {
      *fd=NULL;
      return OAPC_ERROR_DEVICE;
   }
#else
 #ifdef ENV_LINUX
   *fd=open(portname, O_WRONLY);
   if (*fd<0) return OAPC_ERROR_DEVICE;

   if (ioctl(*fd,PPCLAIM)!=0)
   {
      close(*fd);
      *fd=0;
      return OAPC_ERROR_DEVICE;
   }
   ioctl(*fd, LPRESET);			/* generate RESET pulse */
 #else
  #error Not implemented!
 #endif
#endif
   return OAPC_OK;
}



/**
Closes a parallel port and resets its handle/file descriptor so that it can't be used
any longer
@param[in/out] fd handle/file descriptor of the port to be closed
*/
#ifdef ENV_WINDOWS
OAPC_EXT_API void oapc_parallel_port_close(HANDLE *fd)
#else
OAPC_EXT_API void oapc_parallel_port_close(int *fd)
#endif
{
#ifdef ENV_WINDOWS
   CloseHandle(*fd);
   fd=NULL;
#else
   ioctl(*fd,PPRELEASE);
   close(*fd);
   *fd=0;
#endif
}
