/*
 * This software is provided "AS IS," without a warranty of any kind. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NON-INFRINGEMENT, ARE HEREBY EXCLUDED. THE VENDOR AND ITS LICENSORS SHALL
 * NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
 * MODIFYING OR DISTRIBUTING THE SOFTWARE OR ITS DERIVATIVES. IN NO EVENT WILL
 * THE VENDOR OR ITS LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA,
 * OR FOR DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR PUNITIVE
 * DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF LIABILITY, ARISING
 * OUT OF THE USE OF OR INABILITY TO USE SOFTWARE, EVEN IF THE VENDOR HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 *
 * This software is not designed or intended for use in on-line control of
 * aircraft, air traffic, aircraft navigation or aircraft communications; or in
 * the design, construction, operation or maintenance of any nuclear
 * facility. Licensee represents and warrants that it will not use or
 * redistribute the software for such purposes.
 */

#include "oapc_libio.h" // the general header file containing important constants and definitions
#include "liboapc.h"   // the header file for the IServers shared library

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <stddef.h>

#ifdef ENV_LINUX
#include <unistd.h>
#endif

#ifdef ENV_WINDOWS
#include <windows.h>
#endif

static char doExit=0,go=0;


/**
 * This is the callback function that is used to send back data received from the server and that has
 * to be set by calling oapc_set_recv_callback() before a connection is established to the server.
 * When this function is executed new or changed data are received from the IServer or a response is
 * given to the application when specific data have been requested.<BR>
 * The data that are receive here are released by the calling instance so when they are required
 * after this function returned they have to be copied.
 * @param[in] nodeName the name of the data node for which the data are given
 * @param[in] cmd a constant of type OAPC_CMD_xxx that either informs of new data or specifies that
 *            a requested data node could not be found (OAPC_CMDERR_DOESNT_EXISTS)
 * @param[in] a combination of flags (OAPC_DIGI_xxx, OAPX_NUM_xxx, OAPC_CHAR_xxx, OAPC_BIN_xxx) that
 *            specifies what IOs of which type are transmitted within the values
 * @param[in] values the new or changed data of the related node according to the IO-flags, here every
 *            element of the array either points to NULL when this IO doesn't has changed or to a
 *            data structure of type struct oapc_digi_value_block, oapc_num_value_block,
 *            struct oapc_char_value_block or struct oapc_bin_head depending on the related IO flag
 */
static void recv_callback(void *handle,char *nodeName,unsigned int cmd,unsigned int ios,void *values[MAX_NUM_IOS])
{
   if (strncmp(nodeName,"/GOButton/out",MAX_NODENAME_LENGTH)==0)
   {
      struct oapc_digi_value_block *digiValue;
      
		if ((ios & OAPC_DIGI_IO1) && (values[1]))
		{
         digiValue=(struct oapc_digi_value_block*)values[1];
         printf("%s: %d\n",nodeName,digiValue->digiValue);
         go=digiValue->digiValue;
		}
   }
   else if (strncmp(nodeName,"/system/exit",MAX_NODENAME_LENGTH)==0)
   {
      struct oapc_digi_value_block *digiValue;
      
      if ((ios & OAPC_DIGI_IO0) && (values[0]))
      {
         digiValue=(struct oapc_digi_value_block*)values[0];
         printf("Exit: %d\n",digiValue->digiValue);
         if (digiValue->digiValue) doExit=1;
      }
   }
}



/**
 * This function demonstrates some basic usage of the interlock server, it requests and sends some data.
 * ATTENTION: in a real application the return value of every function call has to be checked. When a
 * connection problem occurs and/or data could not be sent, the library closes the connection and returns
 * a value!=OAPC_OK. In this case the complete connection has to be re-established before any other
 * commands can be sent. So there should be following flow of operations:
 * 1. set the receive-callback
 * 2. connect to the server -> exit in case of failure
 * 3. send a command to the server -> go to step 1 in case of failure, continue with other commands elsewhere
 * 4. close the connection before exiting the application
 */
int main(int argc,char *argv[])
{
	void                        *handle;
	struct oapc_digi_value_block digiValue;                           // the structure for digital data
	int                          rc,pos=0,delay1=15,delay2=70;
   char                         myNodeName[MAX_NODENAME_LENGTH+4];
#ifdef ENV_WINDOWS
   WSADATA   wsaData;

   WSAStartup((MAKEWORD(1, 1)), &wsaData);
#endif
   handle=oapc_ispace_get_instance();
   if (handle)
   {
      if (oapc_ispace_set_recv_callback(handle,&recv_callback)==OAPC_OK)        // make the callback-function known to the library
      {
   	   if (oapc_ispace_connect(handle,NULL,0,NULL)==OAPC_OK)                     // connect to the server using default IP and port number
      	{
            while (!doExit)
            {
               if (go)
               {
                  pos++;

                  sprintf(myNodeName,"/Line_%d/in",pos);
	            	digiValue.digiValue=1;
                  rc=oapc_ispace_set_value(handle,myNodeName,OAPC_DIGI_IO0,&digiValue,NULL);
                  oapc_thread_sleep(delay1);

                  if (pos>1) sprintf(myNodeName,"/Line_%d/in",pos-1);
                  else strcpy(myNodeName,"/Line_10/in");
	            	digiValue.digiValue=0;
                  rc=oapc_ispace_set_value(handle,myNodeName,OAPC_DIGI_IO0,&digiValue,NULL);
               }
               oapc_thread_sleep(delay2);
               if (pos>10) pos=0;
            }
      	}
      	else printf("Connection error!\n");
      }
      else printf("Callback registration failed!\n");
   }
	else printf("Instance error!\n");
	
return 0;
}

