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
 * redistribute the Software for such purposes.
 */

#include "oapc_libio.h"     // the general header file containing important constants and definitions
#include "liboapc.h"   // the header file for the interface shared library

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

static unsigned char running=1;



static void recv_callback(int type,char *cmd,unsigned char digi,float num,char *str,struct oapc_bin_head *bin,int socket)
{
	switch (type)
	{
		case OAPC_IFACE_TYPE_STATE_NEW_CONNECTION:
		   printf("New incoming connection accepted\n");
		   break;
		case OAPC_IFACE_TYPE_STATE_CONNECTION_CLOSED:
		   printf("Connection closed\n");
		   break;
		case OAPC_IFACE_TYPE_STATE_CONNECTION_ERROR:
		   printf("Connection failed\n");
		   break;
      case OAPC_IFACE_TYPE_DIGI:
         printf("Digi received:   %s/%d\n",cmd,digi);

         if (!strncmp(cmd,"ECHO_DIGI",MAX_TEXT_LEN))
         {
         	if (oapc_iface_send_digi("REPLY_DIGI",digi,socket)!=OAPC_OK)
         	{
         		running=0;
         		break;
         	}
         }
         else if (!strncmp(cmd,"EXIT",MAX_TEXT_LEN)) running=0;
         break;
      case OAPC_IFACE_TYPE_NUM:
         printf("Number received:   %s/%f\n",cmd,num);

         if (!strncmp(cmd,"ECHO_NUM",MAX_TEXT_LEN))
         {
         	if (oapc_iface_send_num("REPLY_NUM",num,socket)!=OAPC_OK)
         	{
         		running=0;
         		break;
         	}
         }
         break;
      case OAPC_IFACE_TYPE_CHAR:
         printf("String received:   %s/%s\n",cmd,str);

         if (!strncmp(cmd,"ECHO_STR",MAX_TEXT_LEN))
         {
         	if (oapc_iface_send_char("REPLY_STR",str,socket)!=OAPC_OK)
         	{
         		running=0;
         		break;
         	}
         }
         break;
      case OAPC_IFACE_TYPE_BIN:
         break;
      default:
		   printf("ERROR: Unknown state type %d\n",type);
         break;         
	}
}



int main(int argc,char *argv[])
{
	oapc_iface_set_recv_callback(recv_callback);
	if (!oapc_iface_init(NULL,0))
	{
		// could not bind to default IP/port number
		exit(0);
	}

   while (running)
   {
   	oapc_thread_sleep(25);
   }	
   oapc_iface_exit();
   return 0;
}

