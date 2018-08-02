/********************************************************************************************

These sources are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. All
the information given here, within the interface descriptions and within the specification
are subject to change without notice. Errors and omissions excepted.

These sources demonstrate the usage of the OpenAPC Plug-In programming interface. They can be
used freely according to the wxWindows License.

For more information please refer to http://www.wxwindows.org/about/licence3.txt

*********************************************************************************************/

#ifdef ENV_WINDOWS
#pragma warning (disable: 4530)
#endif


#ifdef ENV_WINDOWS
#ifdef _DEBUG 
#include <crtdbg.h> 
#endif
#endif

#include <wx/wx.h>
#include <wx/mstream.h>
#include <wx/zipstrm.h>
#include "bzipstream.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifndef ENV_WINDOWSCE
#include <sys/types.h>
#endif

#ifndef ENV_WINDOWS
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#define closesocket close
#else
#include <windows.h>
#define MSG_NOSIGNAL 0
#endif

#include <zlib.h>

#include "oapc_libio.h"
#include "liboapc.h"

#define MAX_XML_SIZE       5000
#define MAX_FILENAME_SIZE   255
#define MAX_BUFSIZE        5000


struct client_data
{
   struct oapc_bin_head *bin;
   int                   error,input;
   int                   m_callbackID; // TODO: remove this, it is stored 9 times
};


struct instData
{
   struct client_data client[MAX_NUM_IOS];
   int                m_callbackID;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
<flowimage>%s</flowimage>\n\
<dialogue>\n\
 <helppanel>\n\
  <in0>uncompressed data for ZIP compression</in0>\n\
  <in1>uncompressed data for BZ2 compression</in1>\n\
  <in2>uncompressed data for GZ compression</in2>\n\
  <in7>compressed data</in7>\n\
  <out0>ZIP - compressed data</out0>\n\
  <out1>BZ2 - compressed data</out1>\n\
  <out2>GZ - compressed data</out2>\n\
  <out7>uncompressed data</out7>\n\
 </helppanel>\n\
</dialogue>\n\
</oapc-config>\n";
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="iVBORw0KGgoAAAANSUhEUgAAAGoAAAAyCAMAAACDIr4/AAAAA3NCSVQICAjb4U/gAAAAxlBMVEUEAgT8/vyEgoT8/gTEwsT09vzEw8ycnqycnLSZmaySkayIiKSMi5ykpLTU09xiY3yoqLysrr68vczk5exsa4RcYnRcXnlYWHR0coR0c4z09PTExdSEhZzU1uS0tsy8vMTMzdZUU2x8e5SztcTr6uxMTmxjZIRsbYyPkKRMTWTc3eRERVzs7fQ0MkSCg5R4eox0dZQ0NkQsLjw8OkxCQlRUUmQ8PVSsqqxsbnyUlqS0srxcWnykoqw0NkxUWmx8fpwAAAD///+m+GN/AAAACXBIWXMAAA7EAAAOxAGVKw4bAAACYklEQVRYhe2Ya5OaMBSGk1WRixBuCwVpRaFcLdrW2m7dtvr//1RxlRBEHMSsn3wZZzgmc56cc3JhAsAdBSEET71e72n/dtHootLBAdUHuQ36h8Zmo4tKBwfUnl0OvdnoIuzgiMrZ5dCbjS7CDo6onE0MvdnoosJBgeqTQ282uqhwUKBgZejNRhcVBHCzp9bEB6ob4fi7CwrcDQWKHYkeasAMWY7nBWEkSoPzTFooBvGirCiyLKusRhDeASXpz8+Gqev6B0u2CRT1BA4kaSyKjvnxkzxxyQb6UTFT3ZoJPPI8VdJ8TdNqPaihxM+GGYS6HqlsXi3Zk0gE3QQqscUhMzB4LsiD45UEk6hvt2qKRqLrz8ZjSeEcQcH1oo+K0MS358wXFMGB7QmeiltoJzBbTHwI/SSx96tXdRS51oUWykjj2HTE8duCmjtLZVih0ERZX9N4ahooCFnVFcylEr0byl18+776YehhaDn80ghlHmOoT3Z7bXCJ+zP7Zc0ybhbIXK0HNZSbMYeX+WQCXwIRoy5vTOD6zyQX/fZL62XDsqWzC+sKlM2tJaZoESnu4ZzSZhv2FXs7rRXpGnRIq/dnvYqR5XDjBA44cyO81roUxFs/LG119He9jo3wnyB6QbxCOJ2nUVVQgKzVdruD21awuTTU1+k05IVFIpFnSLVWGF1wS9Tb01a+l01XeuVoxK7OTwsICFQeWGvUGdUSCKgtsEvMM38SldvutrSjIpf1Sa12N6FIZs37CQpeMS1aoYiZUC3cjfmroe6h4xUJICJpNjoCAIbAx23M9XrcxlC8jbmb/gPAqzg6BySuXQAAAABJRU5ErkJggg==";
static char                 libname[]="(De)Compress Data";
static char                 xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports



/**
This function has to return the name that is used to display this library within the main application.
The name returned here has to be short and descriptive
@param[out] data pointer to the beginning of an char-array that contains the name
@return the length of the name structure or 0 in case of an error
*/
OAPC_EXT_API char *oapc_get_name(void)
{
   return libname;
}



/**
This function returns a set of OAPC_HAS_... flags that describe the general capabilities of this
library. These flags can be taken from oapc_libio.h
@return or-concatenated flags that describe the libraries capabilities
*/
OAPC_EXT_API unsigned long oapc_get_capabilities(void)
{
   return OAPC_HAS_INPUTS|OAPC_HAS_OUTPUTS|
          OAPC_HAS_XML_CONFIGURATION|
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|OAPC_ACCEPTS_IO_CALLBACK|
          OAPC_FLOWCAT_CONVERSION;
}



/**
When the OAPC_HAS_INPUTS flag is set, the application calls this function to get the configuration
for the inputs. Here "input" is defined from the libraries point of view, means data that are sent from
the application to the library are input data
@return or-concatenated OAPC_..._IO...-flags that describe which inputs and data types are used or 0 in
        case of an error
*/
OAPC_EXT_API unsigned long oapc_get_input_flags(void)
{
   return OAPC_BIN_IO0|OAPC_BIN_IO1|OAPC_BIN_IO2|OAPC_BIN_IO7;
   // avoid it to submit the same IO number for different data types, that would cause displaying problems for the flow symbol
}



/**
When the OAPC_HAS_OUTPUTS flag is set, the application calls this function to get the configuration
for the outputs. Here "output" is defined from the libraries point of view, means data that are sent from
the library to the application are output data
@return or-concatenated OAPC_..._IO...-flags that describe which outputs and data types are used or 0 in case
        of an error
*/
OAPC_EXT_API unsigned long oapc_get_output_flags(void)
{
   return OAPC_BIN_IO0|OAPC_BIN_IO1|OAPC_BIN_IO2|OAPC_BIN_IO7;
}



/**
When the OAPC_HAS_XML_CONFIGURATION capability flag was set this function is called to retrieve an XML
structure that describes which configuration information have to be displayed within the main application.
@param[out] data pointer to the beginning of an char-array that contains the XML data
@return the length of the XML structure or 0 in case of an error
*/
OAPC_EXT_API char *oapc_get_config_data(void* instanceData)
{
/*   struct instData *data;

   data=(struct instData*)instanceData;*/

   sprintf(xmldescr,xmltempl,flowImage);
#ifdef _DEBUG
   int l=strlen(xmldescr);
   l=0;
#endif
   return xmldescr;
}



/**
This function handles all internal data initialisation and has to allocate a memory area where all
data are stored into that are required to operate this Plug-In. This memory area can be used by the
Plug-In freely, it is handed over with every function call so that the Plug-In cann access its
values. The memory area itself is released by the main application, therefore it has to be allocated
using malloc().
@return pointer where the allocated and pre-initialized memory area starts
*/
OAPC_EXT_API void* oapc_create_instance2(unsigned long flags)
{
   flags=flags; // removing "unused" warning

   struct instData *data;

   data=(struct instData*)malloc(sizeof(struct instData));
   memset(data,0,sizeof(struct instData));

   return data;
}



/**
This function is called finally, it has to be used to release the instance data structure that was created
during the call of oapc_create_instance()
*/
OAPC_EXT_API void oapc_delete_instance(void* instanceData)
{
   if (instanceData) free(instanceData);
}



/**
When this function is called everything has to be initialized in order to perform the required operation
@return a return value/error code that informs the main application if the initialization was done successfully
        or not
*/
OAPC_EXT_API unsigned long oapc_init(void*)
{
   return OAPC_OK;
}



/**
This function is called before the application unloads everything, it has to be used to deinitialize
everything and to release used resources.
*/
OAPC_EXT_API unsigned long oapc_exit(void* instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   for (int i=0; i<MAX_NUM_IOS; i++) if (data->client[i].bin) free (data->client[i].bin);
   return OAPC_OK;
}



/**
When the capability flag OAPC_ACCEPTS_IO_CALLBACK is set, the main application no longer cyclically polls
the outputs of a Plug-In and the related parameter within the flow configuration dialogue is turned off.
Instead of this the main application hands over a function pointer to a callback and an ID. Whenever something
changes within the scope of this Plug-In that influences the output state of it, the Plug-In jumps into
that callback function to notify the main application about the new output state. The callback function 
"oapc_io_callback" expects two parameters. The first one "outputs" expects the Or-concatenated flags of
the outputs that have changed and the second one "callbackID" expects the ID that is handed over here to
identify the Plug-In. For a typedef of the callback function oapc_io_callback() that is called by the Plug-In
please refer to oapc_libio.h.<BR><BR>
Here the main application hands over a pointer to the callback function and a unique callback ID. Both have
to be stored for later use
@param[in] oapc_io_callback the callback function that has to be called whenever something changes at the
           outputs of this Plug-In
@param[in] callbackID a unique ID that identifies this Plug-In and that has to be used when the function
           oapc_io_callback is called
*/
OAPC_EXT_API void oapc_set_io_callback(void* instanceData,lib_oapc_io_callback oapc_io_callback,unsigned long callbackID)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   m_oapc_io_callback=oapc_io_callback;
   data->m_callbackID=callbackID;
}



/**
 * Compress given data in GZ-format
 * @param[in] length the size of the data to be compressed
 * @param[in] memin a memory area with the input data that have to be compressed
 * @param[in] memout an allocated memory area with a size of "length" that has to be used
 *            to store compressed data into
 * @return the return value specifies the length of the compressed data, when a value of 0
 *            or a value equal to the given input length is returned, the data could not be
 *            compressed and the contents of memout are unchanged; this may happen in case
 *            the input data already have been compressed and therefore the result would be
 *            bigger than the input
 */
static size_t zcompress(size_t length,void *memin,void *memout)
{
   int      ret;
   z_stream strm;

   strm.zalloc = Z_NULL;
   strm.zfree = Z_NULL;
   strm.opaque = Z_NULL;
   ret = deflateInit2(&strm,Z_BEST_COMPRESSION,Z_DEFLATED,15+16,8,Z_DEFAULT_STRATEGY);
   if (ret != Z_OK) return -1;
   strm.avail_in =length;
   strm.next_in =(Bytef*)memin;
   strm.avail_out =length;
   strm.next_out =(Bytef*)memout;
   ret = deflate(&strm, Z_FINISH);
   assert(ret != Z_STREAM_ERROR);
   deflateEnd(&strm);
   return length-strm.avail_out;
}



/**
 * Decompresses given GZ-compressed data
 * @param[in] length the nlength of the compressed data
 * @param[in] memin memory area containing the compressed data
 * @param[out] memout pointer to a variable where the pointer to the decompressed data
 *            can be returned into, allocation of the data is done by this function,
 *            the caller can release the given memory area with free()
 * @return a value greater 0 when decompression could be performed successful, in this
 *            case the returned value is equal to the size of the data and their memory
 *            area memin; in case an error occured a value smaller or equal 0 is returned,
 *            here -1 specifies a data/decompression error, -3 tells the caller that there
 *            was not enough memory to perform decompression
 */
static int zdecompress(size_t length,void *memin,void **memout)
{
   size_t   have,inLength,outLength=0;
   int      ret;
   z_stream strm;

   strm.zalloc = Z_NULL;
   strm.zfree = Z_NULL;
   strm.opaque = Z_NULL;
   strm.avail_in = 0;
   strm.next_in = Z_NULL;
   ret = inflateInit2(&strm,15+16);
   if (ret != Z_OK) return -1;
   strm.avail_in=length;
   strm.next_in =(Bytef*)memin;

   *memout=malloc(length);
   if (!*memout) return -3;
   inLength=length;
   do
   {
      strm.avail_out =inLength;
      strm.next_out =(Bytef*)(((char*)*memout)+length-inLength);
      ret = inflate(&strm, Z_NO_FLUSH);
      assert(ret != Z_STREAM_ERROR);
      switch (ret)
      {
         case Z_NEED_DICT:
            ret = Z_DATA_ERROR;
         case Z_DATA_ERROR:
            return -1;
         case Z_MEM_ERROR:
            inflateEnd(&strm);
            return -3;
         default:
            break;
      }
      have =inLength-strm.avail_out;
      outLength+=have;
      if (strm.avail_out == 0)
      {
         length=length+inLength;
         *memout=realloc(*memout,length);
         if (!*memout) return -3;
      }
   }
   while (strm.avail_out == 0);
   inflateEnd(&strm);
   return outLength;
}



static void *compressLoop(void *arg)
{
   struct client_data   *client;
   size_t                newLen;
   struct oapc_bin_head *newBin=NULL;

   client=(struct client_data*)arg;	
   client->error=OAPC_OK;

   for (int i=0; i<1; i++)
   {
  	  if (client->input<=1) // compress ZIP/BZ2
	  {
        wxMemoryOutputStream memOut;
   	
	     if (client->input==0)
	     {
          	wxZipOutputStream *zipOut=NULL;

	      	zipOut=new wxZipOutputStream(&memOut,9);

            if (!zipOut)
	        {
               free(client->bin);
               client->bin=NULL;
	       	   client->error=OAPC_ERROR_NO_MEMORY;
   	           break;
	        }
           zipOut->PutNextEntry(_T("data"));
	        zipOut->Write((void*)&client->bin->data,client->bin->sizeData);
	        zipOut->CloseEntry();
           zipOut->Close();
//   	      delete zipOut; deletion is done on destruction of parent memory stream!?
         }
 	      else
 	      {
            wxBZipOutputStream *zipOut;

	         zipOut=new wxBZipOutputStream(memOut,9);

            if (!zipOut)
 	         {
               free(client->bin);
               client->bin=NULL;
	       	   client->error=OAPC_ERROR_NO_MEMORY;
   	         break;
	         }
	         zipOut->Write((void*)&client->bin->data,client->bin->sizeData);
            zipOut->Close();
 	         delete zipOut;
	      }
   
         newLen=memOut.GetSize(); //OutputStreamBuffer()->GetDataLeft();
         newBin=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+newLen);
         if (!newBin)
	      {
            free(client->bin);
            client->bin=NULL;
            client->error=OAPC_ERROR_NO_MEMORY;
   	      break;
  	      }
         memcpy(newBin,client->bin,sizeof(struct oapc_bin_head));
         newBin->sizeData=newLen;
         if (client->input==0) newBin->compression=OAPC_COMPRESS_ZIP;
         else newBin->compression=OAPC_COMPRESS_BZ2;
         memOut.CopyTo(&newBin->data,newLen);
         free(client->bin);
         client->bin=newBin;
	   }
   	else if (client->input==2) // compress GZ
	   {
         void   *memout;
         wxInt32 retLen;

         memout=malloc(client->bin->sizeData);
         if (!memout)
         {
            free(client->bin);
            client->bin=NULL;
            client->error=OAPC_ERROR_NO_MEMORY;
            break;
         }
         retLen=zcompress(client->bin->sizeData,(void*)&client->bin->data,memout);
         if ((retLen<=0) || (retLen==client->bin->sizeData))
         {
  	       	free(memout);
            break;
         }
         newBin=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+retLen);
         if (!newBin)
	      {
            free(client->bin);
            client->bin=NULL;
	   	   client->error=OAPC_ERROR_NO_MEMORY;
   	      break;
   	   }
         memcpy(newBin,client->bin,sizeof(struct oapc_bin_head));
         newBin->sizeData=retLen;
         newBin->compression=OAPC_COMPRESS_GZ;
         memcpy(&newBin->data,memout,retLen);
         free(client->bin);
         free(memout);
         client->bin=newBin;
	   }
      else if (client->input==7) // decompress
	   {
         wxMemoryInputStream  *memIn=NULL;

         if ((client->bin->compression==OAPC_COMPRESS_ZIP) ||
             (client->bin->compression==OAPC_COMPRESS_BZ2))
         {
            memIn=new wxMemoryInputStream((void*)&client->bin->data,client->bin->sizeData);
            if (!memIn)
	         {
               free(client->bin);
               client->bin=NULL;
               client->error=OAPC_ERROR_NO_MEMORY;
   	         break;
	         }
	      }

	      if (client->bin->compression==OAPC_COMPRESS_ZIP)
	      {
          	wxZipInputStream *zipIn;
          	wxZipEntry       *zipEntry;

	      	zipIn=new wxZipInputStream(memIn);

            if (!zipIn)
            {
   	     	   delete memIn;
               free(client->bin);
               client->bin=NULL;
	       	   client->error=OAPC_ERROR_NO_MEMORY;
   	         break;
	         }
            zipEntry=zipIn->GetNextEntry();
            if ((!zipEntry) || (!zipIn->OpenEntry(*zipEntry)))
            {
   	       	delete memIn;
               free(client->bin);
               client->bin=NULL;
	       	   client->error=OAPC_ERROR_INVALID_INPUT;
   	        	break;
            }            

            newLen=zipEntry->GetSize();
            newBin=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+newLen);
            if (!newBin)
	         {
	         	delete memIn;
               free(client->bin);
               client->bin=NULL;
	      	   client->error=OAPC_ERROR_NO_MEMORY;
      	   	break;
      	   }
            memcpy(newBin,client->bin,sizeof(struct oapc_bin_head));
            newBin->sizeData=newLen;

            zipIn->Read(&newBin->data,newLen);
	         zipIn->CloseEntry();
   	      delete zipIn;
	      }
	      else if (client->bin->compression==OAPC_COMPRESS_BZ2)
	      {
          	wxBZipInputStream   *zip;
          	wxMemoryOutputStream memOut;
            char                 buffer[5000];
            
	      	zip=new wxBZipInputStream(*memIn);
            if (!zip)
	         {
   	       	delete memIn;
               free(client->bin);
               client->bin=NULL;
	       	   client->error=OAPC_ERROR_NO_MEMORY;
   	        	break;
	         }
            do
            {
               zip->Read(buffer,5000);
               newLen=zip->LastRead();
               if (newLen==0) break;
               memOut.Write(buffer,newLen);
            }
            while (true);
   	      delete zip;

            newLen=memOut.GetSize();
            newBin=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+newLen);
            if (!newBin)
	         {
	         	delete (zip);
               free(client->bin);
               client->bin=NULL;
	      	   client->error=OAPC_ERROR_NO_MEMORY;
   	      	break;
      	   }
            memcpy(newBin,client->bin,sizeof(struct oapc_bin_head));
            newBin->sizeData=newLen;
            memOut.CopyTo(&newBin->data,newLen);
         }
         else if (client->bin->compression==OAPC_COMPRESS_GZ)
         {
            void   *memout=NULL;
            wxInt32 retLen;

            retLen=zdecompress(client->bin->sizeData,(void*)&client->bin->data,&memout);
            if (retLen<=0)
            {
               free(client->bin);
               client->bin=NULL;
               if (retLen==-3) client->error=OAPC_ERROR_NO_MEMORY;
               else client->error=OAPC_ERROR_INVALID_INPUT;
               if (memout) free(memout);
               break;
            }
            newBin=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+retLen);
            if (!newBin)
            {
               free(memout);
               client->bin=NULL;
               client->error=OAPC_ERROR_NO_MEMORY;
               break;
            }
            memcpy(newBin,client->bin,sizeof(struct oapc_bin_head));
            newBin->sizeData=retLen;
            memcpy(&newBin->data,memout,retLen);
            free(memout);
         }
         else if (client->bin->compression==OAPC_COMPRESS_NONE) // no compression, leave the data unchanged
         {
            break;
	      }
	      else
	      {
	      	delete memIn;
            free(client->bin);
            client->bin=NULL;
	   	   client->error=OAPC_ERROR_INVALID_INPUT; // unsupported compression type
   	   	break;
	      }
   
	      if (newBin)
	      {
            newBin->compression=0;
            free(client->bin);
            client->bin=newBin;
	      }
//   	   delete memIn;
   	}
   }

	if (client->input==0) m_oapc_io_callback(OAPC_BIN_IO0,client->m_callbackID);
	else if (client->input==1) m_oapc_io_callback(OAPC_BIN_IO1,client->m_callbackID);
   else if (client->input==2) m_oapc_io_callback(OAPC_BIN_IO2,client->m_callbackID);
	else if (client->input==7) m_oapc_io_callback(OAPC_BIN_IO7,client->m_callbackID);
	return NULL;
	
}


OAPC_EXT_API unsigned long  oapc_set_bin_value(void* instanceData,unsigned long input,struct oapc_bin_head *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   
   if (data->client[input].bin) return OAPC_ERROR_STILL_IN_PROGRESS;
   if (input==7)
   {
   	if ((value->compression!=OAPC_COMPRESS_ZIP) &&
          (value->compression!=OAPC_COMPRESS_GZ) &&
   	    (value->compression!=OAPC_COMPRESS_BZ2) &&
   	    (value->compression!=OAPC_COMPRESS_NONE)) return OAPC_ERROR_INVALID_INPUT;
   	    // in case a former compression failed and we still have uncompressed data this has to be accepted too
   }
   else if (input>2) return OAPC_ERROR_NO_SUCH_IO;
   
   data->client[input].bin=(struct oapc_bin_head*)malloc(value->sizeData+sizeof(struct oapc_bin_head));
   if (!data->client[input].bin) return OAPC_ERROR_NO_MEMORY;
   memcpy(data->client[input].bin,value,value->sizeData+sizeof(struct oapc_bin_head));
   data->client[input].input=input;
   data->client[input].m_callbackID=data->m_callbackID;
   
   if (!oapc_thread_create(compressLoop,&data->client[input])) return OAPC_ERROR_NO_MEMORY;
   return OAPC_OK;
}



/**
This function is called by the main application as soon as the plug-in triggers it via the callback function.
It has to return the data that are available for that input or - in case there are none available -
the error code OAPC_ERROR_NO_DATA_AVAILABLE to notify the main application, that there is nothing new.
@param[in] output specifies the output where the data are fetched from, here not the OAPC_BIN_IO...-flag is used
           but the plain, 0-based output number
@param[out] value if there are new data available, they have to be stored within that variable, if there are
           no new data, the returned data are ignored by the main application
@return an error code OAPC_ERROR_... in case of an error, OAPC_ERROR_NO_DATA_AVAILABLE in case no new data are
           available or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_get_bin_value(void *instanceData,unsigned long output,struct oapc_bin_head **value)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   	
	if (!data->client[output].bin) return OAPC_ERROR_NO_DATA_AVAILABLE;
	else *value=data->client[output].bin;
	return data->client[output].error;
}



OAPC_EXT_API void oapc_release_bin_data(void *instanceData,unsigned long output)
{
   struct instData *data;

   data=(struct instData*)instanceData;
   	
   free(data->client[output].bin);
	data->client[output].bin=NULL;
}
