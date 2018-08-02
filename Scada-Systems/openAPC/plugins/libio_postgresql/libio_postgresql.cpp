/********************************************************************************************

These sources are distributed in the hope that they will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. All
the information given here, within the interface descriptions and within the specification
are subject to change without notice. Errors and omissions excepted.

These sources demonstrate the usage of the OpenAPC Plug-In programming interface. They can be
used freely according to the OpenAPC Dual License: As long as the sources and the resulting
applications/libraries/Plug-Ins are used together with the OpenAPC software, they are
licensed as freeware. When you use them outside the OpenAPC software they are licensed under
the terms of the GNU General Public License.

For more information please refer to http://www.openapc.com/oapc_license.php

*********************************************************************************************/



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
 #include <winsock2.h>
 #define MSG_NOSIGNAL 0
 #define snprintf _snprintf
#endif

#include <libpq-fe.h>

#include "oapc_libio.h"
#include "liboapc.h"

#define MAX_XML_SIZE     5000
#define MAX_IP_SIZE       100
#define MAX_AUTH_SIZE      20
#define MAX_NAME_SIZE      40
#define MAX_BUF_SIZE     1000

struct libio_config
{
   unsigned short version,length;
   char           ip[MAX_IP_SIZE+1],uname[MAX_AUTH_SIZE+1],pwd[MAX_AUTH_SIZE+1],db[MAX_NAME_SIZE+1];
   unsigned int   reserved1,reserved2,reserved3,reserved4;
   unsigned char  reserved5,reserved6,reserved7,reserved8;
};



struct instData
{
   struct libio_config  config;
   int                  m_callbackID;
   PGconn              *conn;
	char                 m_statement[MAX_BUF_SIZE+1],m_result[MAX_BUF_SIZE+1];
	bool                 m_isRequest,m_running;
};



/** the xml data that define the behaviour and user interface of the plugin within the main application */
static char xmltempl[]={"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<oapc-config>\n\
 <flowimage>%s</flowimage>\n\
 <dialogue>\n\
  <general>\n\
   <param>\n\
    <name>ip</name>\n\
    <text>IP</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>3</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>uname</name>\n\
    <text>Username</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>pwd</name>\n\
    <text>Password</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
   <param>\n\
    <name>db</name>\n\
    <text>Database</text>\n\
    <type>string</type>\n\
    <default>%s</default>\n\
    <min>0</min>\n\
    <max>%d</max>\n\
   </param>\n\
  </general>\n\
  <helppanel>\n\
   <in0>QUER - SQL-query that doesn't returns data</in0>\n\
   <in1>REQ - SQL-query that returns data</in1>\n\
   <out1>RES - result out of SQL-query</out1>\n\
  </helppanel>\n\
 </dialogue>\n\
</oapc-config>\n"};
/** Base64-encoded PNG data of an image that is used as icon for the program flow diagram within the main application*/
static char                 flowImage[]="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";
static char                 libname[]="PostgreSQL Access";
static char                 xmldescr[MAX_XML_SIZE+1];
static lib_oapc_io_callback m_oapc_io_callback; // callback function that is used to inform the main function about changes at the IO ports
static struct libio_config  save_config;



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
          OAPC_ACCEPTS_PLAIN_CONFIGURATION|
          OAPC_ACCEPTS_IO_CALLBACK|
          OAPC_FLOWCAT_DATA;
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
   return OAPC_CHAR_IO0|OAPC_CHAR_IO1;
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
   return OAPC_CHAR_IO1;
}



/**
When the OAPC_HAS_XML_CONFIGURATION capability flag was set this function is called to retrieve an XML
structure that describes which configuration information have to be displayed within the main application.
@param[out] data pointer to the beginning of an char-array that contains the XML data
@return the length of the XML structure or 0 in case of an error
*/
OAPC_EXT_API char *oapc_get_config_data(void* instanceData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   sprintf(xmldescr,xmltempl,flowImage,data->config.ip,   MAX_IP_SIZE,
                                       data->config.uname,MAX_AUTH_SIZE,
                                       data->config.pwd,  MAX_AUTH_SIZE,
                                       data->config.db,   MAX_NAME_SIZE);
   assert(strlen(xmldescr)<MAX_XML_SIZE);
   return xmldescr;
}



/**
When the OAPC_ACCEPTS_PLAIN_CONFIGURATION capability flag was set this function is called for every configurable parameter
to return the value that was set within the application.
@param[in] name the name of the parameter according to the unique identifier that was set within the <name></name> field of the
           XML config file
@param[in] value the value that was configured for this parameter; in case it is not a string but a number it has to be converted,
           the representation sent by the application is always a string
*/
OAPC_EXT_API void oapc_set_config_data(void* instanceData,const char *name,const char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (strcmp(name,"ip")==0)         strncpy(data->config.ip,   value,MAX_IP_SIZE);
   else if (strcmp(name,"uname")==0) strncpy(data->config.uname,value,MAX_AUTH_SIZE);
   else if (strcmp(name,"pwd")==0)   strncpy(data->config.pwd,  value,MAX_AUTH_SIZE);
   else if (strcmp(name,"db")==0)    strncpy(data->config.db,   value,MAX_NAME_SIZE);
}



/**
This function delivers the data that are stored within the project file by the main application. It is
recommended to put two fields "version" and "length" into the data structure that is handed over here.
So when the data to be saved need to be changed it is easy to recognize which version of the data structure
is used, possible conversions can be done easier in oapc_set_loaded_data().<BR>
PLEASE NOTE: In order to keep the resulting project files compatible with all possible platforms the
             application is running at you have to store all values in network byteorder
@param[out] length the total effective length of the data that are returned by this function
@return the data that have to be saved
*/
OAPC_EXT_API char *oapc_get_save_data(void* instanceData,unsigned long *length)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   *length=sizeof(struct libio_config);
   save_config.version=htons(1);
   save_config.length =htons((unsigned short)*length);
   strncpy(save_config.ip,   data->config.ip,   MAX_IP_SIZE);
   strncpy(save_config.uname,data->config.uname,MAX_AUTH_SIZE);
   strncpy(save_config.pwd,  data->config.pwd,  MAX_AUTH_SIZE);
   strncpy(save_config.db,   data->config.db,   MAX_NAME_SIZE);
   save_config.reserved1   =htonl(data->config.reserved1);
   save_config.reserved2   =htonl(data->config.reserved2);
   save_config.reserved3   =htonl(data->config.reserved3);
   save_config.reserved4   =htonl(data->config.reserved4);
   save_config.reserved5   =data->config.reserved5;
   save_config.reserved6   =data->config.reserved6;
   save_config.reserved7   =data->config.reserved7;
   save_config.reserved8   =data->config.reserved8;

   return (char*)&save_config;
}



/**
This function receives the data that have been stored within the project file by the main application. It is
recommended to check if the returned data are really what the library expects. To do that two fields
"version" and "length" within the saved data structure should be checked.<BR>
PLEASE NOTE: In order to keep the resulting project files compatible with all possible platforms the
             application is running at you have to convert all the values that have been stored in network
             byteorder back to the local byteorder
@param[in] length the total effective length of the data that are handed over to this function
@param[in] data the configuration data that are loaded for this library
*/
OAPC_EXT_API void oapc_set_loaded_data(void* instanceData,unsigned long length,char *loadedData)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (length>sizeof(struct libio_config)) length=sizeof(struct libio_config);
   memcpy(&save_config,loadedData,length);
   data->config.version=ntohs(save_config.version);
   data->config.length =ntohs(save_config.length);
   strncpy(data->config.ip,   save_config.ip,   MAX_IP_SIZE);
   strncpy(data->config.uname,save_config.uname,MAX_AUTH_SIZE);
   strncpy(data->config.pwd,  save_config.pwd,  MAX_AUTH_SIZE);
   strncpy(data->config.db,   save_config.db,   MAX_NAME_SIZE);
   data->config.reserved1=ntohl(save_config.reserved1);
   data->config.reserved2=ntohl(save_config.reserved2);
   data->config.reserved3=ntohl(save_config.reserved3);
   data->config.reserved4=ntohl(save_config.reserved4);
   data->config.reserved5=save_config.reserved5;
   data->config.reserved6=save_config.reserved6;
   data->config.reserved7=save_config.reserved7;
   data->config.reserved8=save_config.reserved8;
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
This function handles all internal data initialisation and has to allocate a memory area where all
data are stored into that are required to operate this Plug-In. This memory area can be used by the
Plug-In freely, it is handed over with every function call so that the Plug-In cann access its
values. The memory area itself is released by the main application, therefore it has to be allocated
using malloc().
@return pointer where the allocated and pre-initialized memory area starts
*/
OAPC_EXT_API void* oapc_create_instance2(unsigned long flags)
{
   struct instData *data;

   flags=flags; // removing "unused" warning
   data=(struct instData*)malloc(sizeof(struct instData));
   memset(data,0,sizeof(struct instData));

   data->config.version=1;
   data->config.length=sizeof(struct libio_config);
   data->m_running=true;
   strcpy(data->config.ip,"127.0.0.1");
   strcpy(data->config.uname,"postgres");
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
OAPC_EXT_API unsigned long oapc_init(void* instanceData)
{
   struct instData   *data;
   char               connectStr[300+1];

   data=(struct instData*)instanceData;

   snprintf(connectStr,300,"host=%s dbname=%s",data->config.ip,data->config.db);
   if ((data->config.uname[0]!=0) && (strlen(data->config.uname)+strlen(connectStr)+6<300))
   {
      strcat(connectStr," user=");
      strcat(connectStr,data->config.uname);
   }
   if ((data->config.pwd[0]!=0) && (strlen(data->config.pwd)+strlen(connectStr)+10<300))
   {
      strcat(connectStr," password=");
      strcat(connectStr,data->config.pwd);
   }
   data->conn=PQconnectdb(connectStr);
   if ((!data->conn) || (PQstatus(data->conn)!=CONNECTION_OK))
   {
      printf("%s %d Connect failed: %s (%s)\n",__FILE__,__LINE__,PQerrorMessage(data->conn),connectStr);
      data->conn=NULL;
      return OAPC_ERROR_CONNECTION;
   }
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
   data->m_running=false;
   if (data->conn)
   {
      oapc_thread_sleep(500);
      PQfinish(data->conn);
   }
   return OAPC_OK;
}



static void *queryLoop(void *arg)
{
   struct instData *data;
   PGresult        *result;
   unsigned int     num_fields,i,rows;
   bool             escape;
   char            *c;

   data=(struct instData*)arg;

   result=PQexec(data->conn,data->m_statement);
   if (!result) 
   {
      printf("%s %d %s\n",__FILE__,__LINE__,PQerrorMessage(data->conn));
      data->m_statement[0]=0;
      return NULL;
   }
   if( (data->m_isRequest) && (data->m_running))
   {
      if ((PQresultStatus(result)!=PGRES_TUPLES_OK) || (PQntuples(result)<=0))
      {
         // no result
         data->m_result[0]=0;
         m_oapc_io_callback(OAPC_CHAR_IO1,data->m_callbackID);
      }
      else
      {
         rows=PQntuples(result);
         num_fields=PQnfields(result);
         for (i=0; i<rows; i++)
         {
            if (!data->m_running) break;
            c=PQgetvalue(result,rows+1,num_fields+1);
            if (strlen(data->m_result)+strlen(c)+4<MAX_BUF_SIZE)
            {
               if (i>0) strcat(data->m_result,"; ");
               if (strstr(c,"\""))
               {
                  escape=true;
                  strcat(data->m_result,"\"");
               }
               else escape=false;
               strcat(data->m_result,c);
               if (escape) strcat(data->m_result,"\"");
            }
         }
         m_oapc_io_callback(OAPC_CHAR_IO1,data->m_callbackID);
      }
   }
   PQclear(result);
   data->m_statement[0]=0;
   return NULL;
}


/**
This function is called by the main application when the library provides an character input (marked
using the digital input flags OAPC_CHAR_IO...), a connection was edited to this input and a data
flow reaches the input.
@param[in] input specifies the input where the data are send to, here not the OAPC_CHAR_IO...-flag is used
           but a plain, 0-based input number
@param[in] value specifies the string that is set to that input
@return an error code OAPC_ERROR_... in case of an error or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_set_char_value(void* instanceData,unsigned long input,char *value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (data->m_statement[0]!=0) return OAPC_ERROR_STILL_IN_PROGRESS;
   data->m_callbackID=data->m_callbackID; // TODO: remove double usage of this variable
   strncpy(data->m_statement,value,MAX_BUF_SIZE);
   if (input==0) // query
   {
   	data->m_isRequest=false;
      if (!oapc_thread_create(queryLoop,data)) return OAPC_ERROR_NO_MEMORY;
      return OAPC_OK;
   }
   else if (input==1) // request
   {
   	data->m_isRequest=true;
      if (!oapc_thread_create(queryLoop,data)) return OAPC_ERROR_NO_MEMORY;
      return OAPC_OK;
   }
   else return OAPC_ERROR_NO_SUCH_IO;
}



/**
This function is called by the main application periodically in order to poll the state of the related
output. It has to return the data that are available for that input or - in case there are none available -
the error code OAPC_ERROR_NO_DATA_AVAILABLE to notify the main application, that there is nothing new.
@param[in] output specifies the output where the data are fetched from, here not the OAPC_CHAR_IO...-flag is used
           but the plain, 0-based output number
@param[out] value if there are new data available, they are stored within that variable, if there are no new
           data, the returned data are ignored by the main application
@return an error code OAPC_ERROR_... in case of an error, OAPC_ERROR_NO_DATA_AVAILABLE in case no new data are
           available or OAPC_OK in case the value could be set
*/
OAPC_EXT_API unsigned long  oapc_get_char_value(void *instanceData,unsigned long output,unsigned long length,char* value)
{
   struct instData *data;

   data=(struct instData*)instanceData;

   if (output==1)
   {
   	strncpy(value,data->m_result,length);
   	return OAPC_OK;
   }
   return OAPC_ERROR_NO_SUCH_IO;
}


