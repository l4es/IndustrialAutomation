/******************************************************************************

This file is part of ControlRoom process control/HMI software.

ControlRoom is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

ControlRoom is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
ControlRoom. If not, see http://www.gnu.org/licenses/

*******************************************************************************

For different licensing and/or usage of the sources apart from GPL or any other
open source license, please contact us at https://openapc.com/contact.php

*******************************************************************************/

#ifdef ENV_WINDOWS
 #pragma warning (disable: 4996)
#endif

#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>


#ifdef ENV_LINUX
 #include <arpa/inet.h>
#endif

#ifdef ENV_WINDOWS
 #include <windows.h>
 #define vsnprintf _vsnprintf
 #define snprintf _snprintf
#endif

#include <list>

#include "common.h"
#include "iff.h"
#include "oapc_libio.h" // the general header file containing important constants and definitions
#include "liboapc.h"    // the header file for the IServers shared library

static bool verbose=false;
#ifndef ENV_WINDOWS
 #include <dlfcn.h>
#endif

#define PING_DELAY_TIME 1

typedef unsigned long  (*lib_oapc_get_capabilities)(void);
typedef unsigned long  (*lib_oapc_get_output_flags)(void);
typedef void           (*lib_oapc_set_loaded_data) (void* instanceData,unsigned long length,char *data);
typedef void*          (*lib_oapc_create_instance2)(unsigned long flags);
typedef unsigned long  (*lib_oapc_init)            (void* instanceData);
typedef unsigned long  (*lib_oapc_exit)            (void* instanceData);
typedef void           (*lib_oapc_delete_instance) (void* instanceData);
typedef void           (*lib_oapc_set_io_callback) (void* instanceData,lib_oapc_io_callback oapc_io_callback,unsigned long callbackID);
typedef unsigned long  (*lib_oapc_set_digi_value)  (void* instanceData,unsigned long input,unsigned char value);
typedef unsigned long  (*lib_oapc_get_digi_value)  (void* instanceData,unsigned long output,unsigned char *value);
typedef unsigned long  (*lib_oapc_set_num_value)   (void* instanceData,unsigned long input,double value);
typedef unsigned long  (*lib_oapc_get_num_value)   (void* instanceData,unsigned long output,double *value);
typedef unsigned long  (*lib_oapc_set_char_value)  (void* instanceData,unsigned long input,char *value);
typedef unsigned long  (*lib_oapc_get_char_value)  (void* instanceData,unsigned long output,unsigned long length,char *value);
typedef unsigned long  (*lib_oapc_set_bin_value)   (void* instanceData,unsigned long input,struct oapc_bin_head *value);
typedef unsigned long  (*lib_oapc_get_bin_value)   (void* instanceData,unsigned long output,struct oapc_bin_head **value);
typedef unsigned long  (*lib_oapc_release_bin_data)(void* instanceData,unsigned long output);

static lib_oapc_get_capabilities fn_oapc_get_capabilities;
static lib_oapc_get_output_flags fn_oapc_get_output_flags;
static lib_oapc_set_loaded_data  fn_oapc_set_loaded_data;
static lib_oapc_create_instance2 fn_oapc_create_instance2;
static lib_oapc_delete_instance  fn_oapc_delete_instance;
static lib_oapc_init             fn_oapc_init;
static lib_oapc_exit             fn_oapc_exit;
static lib_oapc_set_digi_value   fn_oapc_set_digi_value;
static lib_oapc_get_digi_value   fn_oapc_get_digi_value;
static lib_oapc_set_num_value    fn_oapc_set_num_value;
static lib_oapc_get_num_value    fn_oapc_get_num_value;
static lib_oapc_set_char_value   fn_oapc_set_char_value;
static lib_oapc_get_char_value   fn_oapc_get_char_value;
static lib_oapc_set_bin_value    fn_oapc_set_bin_value;
static lib_oapc_get_bin_value    fn_oapc_get_bin_value;
static lib_oapc_release_bin_data fn_oapc_release_bin_data;
static lib_oapc_set_io_callback  fn_oapc_set_io_callback;

struct recvDataBlock
{
   unsigned int  IOMask,IONum;
   union td
   {
      unsigned char         digiValue;
      double                numValue;
      char                  charValue[MAX_TEXT_LEN+4];
      struct oapc_bin_head *binValue;
   } d;
};

static char                   *m_file=NULL,*m_devName=NULL,*m_playerWorkingDirectory=NULL;
static char                    m_inNodeName[MAX_NODENAME_LENGTH+4],m_outNodeName[MAX_NODENAME_LENGTH+4];
static int                     m_devID=0;
static void                   *dynLib=NULL;
static void                   *m_pluginData=NULL; // instance data
static void                   *m_isHandle=NULL;   // Interlock Server handle
static hmiProjectData          projectData;
static bool                    doExit=false,messageSent=false;
static unsigned long           m_outputFlags=0;
static struct externalFlowData flowData;
static std::list<struct recvDataBlock*> recvList;
static void                   *recvMutex=NULL;
#ifdef _DEBUG
 static FILE                   *FHandle=NULL;



void writeLog(const char *format,...)
{
   va_list  arglist;
   char     sText[1500];
   void    *time;
   double   sec=0;

   if (!FHandle) return;
   va_start(arglist,format);
   vsprintf(sText,format,arglist);
   va_end(arglist);
   time=oapc_util_get_time();
   if (time)
   {
      sec=oapc_util_get_timeofday(time);
      oapc_util_release_time(time);
   }
#ifdef ENV_WINDOWS
   fprintf(FHandle,"%f\t%s\r\n",sec,sText);
#else
   fprintf(FHandle,"%f\t%s\n",sec,sText);
#endif
}
#endif // _DEBUG




/**
 * Release all (possibly) used resources
 */
static void releaseAll(void)
{
   if (m_file) free(m_file);
   m_file=NULL;
   if (m_devName) free(m_devName);
   m_devName=NULL;
   if (m_playerWorkingDirectory) free(m_playerWorkingDirectory);
   m_playerWorkingDirectory=NULL;
   if (m_isHandle) oapc_ispace_disconnect(m_isHandle);
   m_isHandle=NULL;
   if ((fn_oapc_exit) && (m_pluginData)) fn_oapc_exit(m_pluginData);
   if ((fn_oapc_delete_instance) && (m_pluginData)) fn_oapc_delete_instance(m_pluginData);
   m_pluginData=0;
   if (dynLib) oapc_dlib_release(dynLib);
   dynLib=NULL;
   if (recvMutex) oapc_thread_mutex_release(recvMutex);
   recvMutex=NULL;
}



/**
 * Callback function for receiving data from the Interlock Server, here all changed data are announced
 * and handled if they belong to the controlled Plug-in; in case the exit message in /system/exit arrives
 * the plugger is ended too
 */
static void recv_callback(void *handle,char *nodeName,unsigned int cmd,unsigned int ios,void *values[MAX_NUM_IOS])
{
   if (verbose)
   {
      printf("Getting data from Interlock Server, node %s, cmd %d, ios 0x%X\n",nodeName,cmd,ios);
#ifdef _DEBUG
      writeLog("Getting data from Interlock Server, node %s, cmd %d, ios 0x%X\n",nodeName,cmd,ios);
#endif
   }
   if (strncmp(nodeName,m_inNodeName,MAX_NODENAME_LENGTH)==0)
   {
      int                   i;
      unsigned int          bitmask =0x01010101;
      struct recvDataBlock *recvData;
      
      for (i=0; i<MAX_NUM_IOS; i++)
      {
         recvData=NULL;
   		if ((ios & OAPC_DIGI_IO_MASK & bitmask) && (values[i]))
	   	{
   	      struct oapc_digi_value_block *digiValue;

   	      digiValue=(struct oapc_digi_value_block*)values[i];
            if (fn_oapc_set_digi_value)
            {
//             fn_oapc_set_digi_value(m_pluginData,i,digiValue->digiValue);
               recvData=(struct recvDataBlock*)malloc(sizeof(struct recvDataBlock));
               if (recvData)
               {
                  recvData->IOMask=OAPC_DIGI_IO_MASK;
                  recvData->d.digiValue=digiValue->digiValue;
               }
            }
	   	}
   		else if ((ios & OAPC_NUM_IO_MASK & bitmask) && (values[i]))
         {
   	      struct oapc_num_value_block *numValue;

   	      numValue=(struct oapc_num_value_block*)values[i];
            if (fn_oapc_set_num_value)
            {
//             fn_oapc_set_num_value(m_pluginData,i,d);
               recvData=(struct recvDataBlock*)malloc(sizeof(struct recvDataBlock));
               if (recvData)
               {
                  recvData->IOMask=OAPC_NUM_IO_MASK;
                  recvData->d.numValue=oapc_util_block_to_dbl(numValue);
//printf("Plugger: receiving %f\n",recvData->d.numValue);
               }
            }
         }
   		else if ((ios & OAPC_CHAR_IO_MASK & bitmask) && (values[i]))
         {
            struct oapc_char_value_block *charValue;

            charValue=(struct oapc_char_value_block*)values[i];
            if (fn_oapc_set_char_value)
            {
//             fn_oapc_set_char_value(m_pluginData,i,charValue->charValue);
               recvData=(struct recvDataBlock*)malloc(sizeof(struct recvDataBlock));
               if (recvData)
               {
                  recvData->IOMask=OAPC_CHAR_IO_MASK;
                  strncpy(recvData->d.charValue,charValue->charValue,MAX_TEXT_LEN);
               }
            }
         }
         else if ((ios & OAPC_BIN_IO_MASK & bitmask) && (values[i]))
         {
            struct oapc_bin_head *bin=NULL;

            bin=(struct oapc_bin_head*)values[i];
            if (fn_oapc_set_bin_value)
            {
//             fn_oapc_set_bin_value(m_pluginData,i,bin);
               recvData=(struct recvDataBlock*)malloc(sizeof(struct recvDataBlock));
               if (recvData)
               {
                  recvData->IOMask=OAPC_BIN_IO_MASK;
                  recvData->d.binValue=(struct oapc_bin_head*)malloc(sizeof(struct oapc_bin_head)+bin->sizeData);
                  if (!recvData->d.binValue)
                  {
                     free(recvData);
                     recvData=NULL;
                  }
                  else memcpy(recvData->d.binValue,bin,sizeof(struct oapc_bin_head)+bin->sizeData);
               }
            }
         }
   		if (recvData)
   		{
#ifdef _DEBUG
            writeLog("putting data for %s into list",nodeName);
#endif
            recvData->IONum=i;
            oapc_thread_mutex_lock(recvMutex);
            recvList.push_back(recvData);
            oapc_thread_mutex_unlock(recvMutex);
   		}
   		bitmask=bitmask<<1;
	   }
   }
   else if (strncmp(nodeName,"/system/exit",MAX_NODENAME_LENGTH)==0)
   {
      struct oapc_digi_value_block *digiValue;
      
      if ((ios & OAPC_DIGI_IO0) && (values[0]))
      {
         digiValue=(struct oapc_digi_value_block*)values[0];
         if (digiValue->digiValue) doExit=true;
      }
   }
}



/** definitions for the command line options */
static char shortopts[] = "vVhp:n:i:w:";
#ifndef ENV_QNX
static struct option const longopts[] = {
  {"version",          no_argument, NULL, 'v'},
  {"verbose",          no_argument, NULL, 'V'},
  {"help",             no_argument, NULL, 'h'},
//  {"high-availability",no_argument, NULL, 'a'},
  {NULL, no_argument, NULL, 0}
};
#endif



/** definitions for the command line help output */
#ifdef ENV_QNX
static char const *const option_help[] = {
  " -p   project file to load",
  " -n   unique name of the device to be handled",
  " -i   unique ID of the device to be handled",
  " -V   print out some debugging information",
  " -v   show version info",
  " -h   show this help",
  0
};
#else
static char const *const option_help[] = {
  " -p   project file to load",
  " -n   unique name of the device to be handled",
  " -i   unique ID of the device to be handled",
  #ifdef ENV_LINUX
  " -V   print out some debugging information",
  #endif
  " -v   show version info",
  " -h   show this help",
  0
};
#endif



/** print out version information */
static void version(void)
{
   printf ("OpenPlugger v%s / http://www.openapc.com\n(c) 2010-2013 by OpenAPC Project Group\n",GLOBAL_VERSION);
}



/** print out command line help information */
static void usage (char *pname)
{
   printf ("Usage:   %s [OPTIONS]\n",pname);
   printf ("Example: %s -p myproject.apcx -n RobotController_32\n",pname);
   printf ("or:      %s -p myproject.apcx -i 29\n",pname);
   printf (" -h for more information\n\n");
   return;
}



/**
 * Get the switches and related values out of the command line parameters
 * @param argc the number of arguments as handed over by main()
 * @param argv the arguments as handed over by main()
 */
static bool getSwitches(int argc,char *argv[])
{
   int                optc;
   char const *const *p;

   if (optind == argc)
   {
      usage (argv[0]);
      return false;
   }
#ifndef ENV_WINDOWS
while ((optc = getopt(argc, argv, shortopts)) != -1)
#else
while ((optc = getopt_long (argc, argv, shortopts, longopts, (int *) 0)) != -1)
#endif
   {
   switch (optc)
      {
      case 'V':
         verbose=true;
         break;
      case 'v':
         version();
         exit (0);
         break;
      case 'h':
         usage (argv[0]);
         for (p = option_help; *p; p++) printf ("%s\n", *p);
         exit (0);
         break;
      case 'p':
         m_file=strdup(optarg);
         break;
      case 'i':
         m_devID=atoi(optarg);
         break;
      case 'n':
          m_devName=strdup(optarg);
         break;
      case 'w':
          m_playerWorkingDirectory=strdup(optarg);
         break;
      default:
         usage (argv[0]);
      }
   }
   if (!m_file)
   {
      printf("\nERROR: no APCP project file specified!\n\n");
      usage (argv[0]);
      return false;
   }
   if ((!m_devName) && (m_devID<=0))
   {
      printf("\nERROR: no plug-in name and no plug-in ID specified!\n\n");
      usage (argv[0]);
      return false;
   }
   return true;
}



/**
 * Handling of messages
 * @param[in] txt the message with printf-style format options
 * @param[in) ... additional parameters
 */
static void gMessage(const char *txt,...)
{
   char buffer[400+1];
   va_list az;

   va_start(az,txt);
   vsnprintf(buffer, sizeof(buffer) - 1,txt, az);
   va_end(az);
   printf("%s\n",buffer);
#ifdef ENV_WINDOWS
   if (verbose)
    MessageBox(NULL,buffer,"OpenPlugger Message",MB_OK);
#endif
}



/**
 * In-place conversion of a UTF16BE-encoded wide char string to a plain ASCII text; this
 * conversion is somewhat primitive, it simply assumes that only the lower 8 byte of the
 * wide char string contain relevant data that can be mapped to local ASCII directly.
 * @param[in/out] utf char pointer to the wide char data of the UTF16 string
 * @param[in] len the length (in charaters) of the string
 */
static void utf16BEToASCII(char *utf,int len)
{
   int s,d;

   d=0;
   for (s=1; s<len/2; s+=2)
   {
      utf[d]=utf[s];
      d++;
   }
}



/**
 * Loads the EXIO chunk plug-in data out of the project file and hand them over to
 * the plug-in
 * @param[in] FHandle file handle of the opened project file where the read pointer
 *            is positioned at the beginning of the plug-in specific data of the
 *            EXIO chunk (and not at the beginning of the chunk itself!)
 * @param[in] flowData the externalFlowData structure that has been read and decoded
 *            already
 * @param[out] ok true in case the plug-in-specific data could be read and set
 *            successfully, false otherwise
 */
static int loadEXIO(FILE *FHandle,struct externalFlowData *flowData,bool *ok)
{
   size_t loaded=0;
   int    rcode;
   void  *extLibData;

   *ok=true;
   fseek(FHandle,sizeof(flowData->flowData),SEEK_CUR); // skip flow data
   extLibData=malloc(flowData->extLibDataSize);
   if (extLibData)
   {
      loaded=loaded+fread(extLibData,1,flowData->extLibDataSize,FHandle);
      if (fn_oapc_set_loaded_data) fn_oapc_set_loaded_data(m_pluginData,flowData->extLibDataSize,(char*)extLibData);
      free(extLibData);
   }
   else
   {
      if (fseek(FHandle,flowData->extLibDataSize,SEEK_CUR)>0) loaded+=flowData->extLibDataSize;
   }
   if (fn_oapc_init)
   {
      rcode=fn_oapc_init(m_pluginData);
      if (rcode!=OAPC_OK)
      {
         void                        *values[MAX_NUM_IOS];
         struct oapc_num_value_block  numValue;
         struct oapc_char_value_block charNameValue,charCustomValue;

         gMessage("Initialisation of Plug-In failed (%d)",rcode);

         oapc_util_dbl_to_block(rcode,&numValue);
         values[0]=&numValue;

         memcpy(charNameValue.charValue,flowData->store_name,MAX_TEXT_LEN);
         values[1]=&charNameValue;

         if (rcode==OAPC_ERROR_CUSTOM)
         {
            typedef unsigned long (*lib_oapc_get_error_message)(void *instanceData,unsigned long length,char* value);

            lib_oapc_get_error_message fn_oapc_get_error_message;

            fn_oapc_get_error_message=(lib_oapc_get_error_message)oapc_dlib_get_symbol(dynLib,"oapc_get_error_message");

            if (fn_oapc_get_error_message) fn_oapc_get_error_message(m_pluginData,MAX_TEXT_LEN,charCustomValue.charValue);
            else strncpy(charCustomValue.charValue,"An undefined error occurred",MAX_TEXT_LEN);
            values[2]=&charCustomValue;

            oapc_ispace_set_data(m_isHandle,"/system/pluggererr",OAPC_NUM_IO0|OAPC_CHAR_IO1|OAPC_CHAR_IO2,values,NULL);
         }
         else oapc_ispace_set_data(m_isHandle,"/system/pluggererr",OAPC_NUM_IO0|OAPC_CHAR_IO1,values,NULL);
         *ok=false;
      }
   }
   else
   {
      gMessage("Initialisation of Plug-In failed, no init-function found!");
      *ok=false;
   }
   return (int)loaded;
}



/**
 * Loads a project file (APCP or APCX) and fetches the relevant information out of it
 * @param[in] file the file name of the project as handed over by the command line parameter
 * @param[in] playerWorkingDirectory the working directory of the player/debugger that executed
 *            the plugger; this parameter is used in case the file specified in parameter "file"
 *            could not be found, in this case the working directory is prepended to it in
 *            order to build an absolute path to the file
 * return true in case the project file could be loaded successfully, false otherwise
 */
static bool loadProject(char *file,char *playerWorkingDirectory)
{
   FILE  *FHandle;
   char   rawData[5];
   size_t readSize;
   int    chunkSize;

   FHandle=fopen(file,"rb");
   if ((!FHandle) && (playerWorkingDirectory))
   {
      char *path;
      
      path=(char*)malloc(strlen(file)+strlen(playerWorkingDirectory)+1+4);
      if (path)
      {
#ifdef ENV_WINDOWS
         sprintf(path,"%s\\%s",playerWorkingDirectory,file);
#else
         sprintf(path,"%s/%s",playerWorkingDirectory,file);
#endif
         FHandle=fopen(path,"rb");
         free(path);
      }
   }
   if (FHandle)
   {
      rawData[4]=0;
      fread(rawData,1,4,FHandle);
      if (strncmp(rawData,CHUNK_FORM,4))
      {
         fclose(FHandle);
         gMessage("No valid IFF file!");
	      return false;
      }
      fseek(FHandle,4,SEEK_CUR); // TODO: evaluate length information
      fread(rawData,1,4,FHandle);
      if (strncmp(rawData,CHUNK_APCP,4))
      {
         if (strncmp(rawData,CHUNK_APCX,4))
	      {
            fclose(FHandle);
            gMessage("No valid APCP project file or APCX compiled project (%s)!",rawData);
            return false;
         }
      }
      while (1)
      {
         readSize=fread(rawData,1,4,FHandle);
         readSize+=fread(&chunkSize,1,4,FHandle);
         if (readSize!=8) break;
         chunkSize=ntohl(chunkSize);

         if (chunkSize>0)
         {
            if (!strncmp(rawData,CHUNK_PROJ,4))
            {
               wxUint32       l;

               l=chunkSize;
               if (l>sizeof(struct hmiProjectData)) l=sizeof(struct hmiProjectData);
               chunkSize-=(int)fread(&projectData,1,l,FHandle);
               projectData.version        =ntohl(projectData.version);
               projectData.flags          =ntohl(projectData.flags);
/*	                    m_projectData->bgCol          =ntohl(m_projectData->bgCol);
	                    m_projectData->gridW          =ntohl(m_projectData->gridW);
	                    m_projectData->gridH          =ntohl(m_projectData->gridH);
	                    m_projectData->totalW         =ntohl(m_projectData->totalW);
	                    m_projectData->totalH         =ntohl(m_projectData->totalH);
	                    m_projectData->flowW          =ntohl(m_projectData->flowW);
	                    m_projectData->flowH          =ntohl(m_projectData->flowH);
	                    m_projectData->flowTimeout    =ntohl(m_projectData->flowTimeout);
	                    m_projectData->timerResolution=ntohl(m_projectData->timerResolution);
	                    m_projectData->editorX        =ntohl(m_projectData->editorX);
	                    if (m_projectData->editorX<0) m_projectData->editorX=0;
	                    else if (m_projectData->editorX>2000) m_projectData->editorX=2000;
	                    m_projectData->editorY        =ntohl(m_projectData->editorY);
	                    if (m_projectData->editorY<0) m_projectData->editorY=0;
	                    else if (m_projectData->editorY>1000) m_projectData->editorY=1000;
	                    m_projectData->editorW        =ntohl(m_projectData->editorW);
	                    if (m_projectData->editorW<200) m_projectData->editorW=200;
	                    else if (m_projectData->editorW>2000) m_projectData->editorW=2000;
	                    m_projectData->editorH        =ntohl(m_projectData->editorH);
	                    if (m_projectData->editorH<100) m_projectData->editorH=100;
	                    else if (m_projectData->editorH>2000) m_projectData->editorH=2000;
	                    if (m_projectData->version>=2)
	                    {
	                       // new since 1.1
	                       m_projectData->touchFactor    =ntohl(m_projectData->touchFactor);
	                       if (m_projectData->touchFactor<1000.0) m_projectData->touchFactor=1000.0;
	                       else if (m_projectData->touchFactor>20000.0) m_projectData->touchFactor=20000.0;
	                       g_projectTouchFontFaceName=g_charToStringUTF16BE(m_projectData->font.store_faceName,sizeof(m_projectData->font.store_faceName));
	                       m_projectData->font.pointSize=ntohl(m_projectData->font.pointSize);
	                       m_projectData->font.style=ntohl(m_projectData->font.style);
	                       m_projectData->font.weight=ntohl(m_projectData->font.weight);
	                    }
	   #endif*/
               if (projectData.version>=3)
	           {
	              // new since 1.3
	              //m_projectData->m_remSingleIP
               }
            }
            else if (!strncmp(rawData,CHUNK_DEVL,4))
            {
               wxInt32     subChunkSize;

           	   while (chunkSize>0)
           	   {
           	      readSize=fread(rawData,1,4,FHandle);
           	      readSize+=fread(&subChunkSize,1,4,FHandle);
           	      if (readSize!=8) break;
           	      chunkSize-=8;
           	      subChunkSize=ntohl(subChunkSize);
           	      assert(subChunkSize>0);
//           	      if (subChunkSize<=0) return chunkSize; !!!
           	      assert(subChunkSize<=1000000);
//           	      if (subChunkSize>1000000) return chunkSize; !!!

           	      chunkSize-=subChunkSize; // the following code ensures that exactly the size of subChunkSize is read so we can substract it here
           	      if (!strncmp(rawData,CHUNK_FLOW_EXIO,4)) // external IO flow objects
           	      {
#ifdef ENV_WINDOWS
           	         char libPath[MAX_LIBNAME_LENGTH+4];
#else
           	         char libPath[MAX_LIBNAME_LENGTH+strlen(DATA_PATH)+4];
#endif

           	         subChunkSize-=(wxInt32)fread(&flowData,1,sizeof(struct externalFlowData),FHandle);
           	         flowData.flowData.version=  ntohl(flowData.flowData.version);
           	         flowData.flowData.id  =ntohl(flowData.flowData.id);           	         
           	         flowData.flowData.usedFlows=ntohl(flowData.flowData.usedFlows);
           	         flowData.flowData.maxEdges= ntohl(flowData.flowData.maxEdges);
           	         flowData.flowX=             ntohl(flowData.flowX);
           	         flowData.flowY=             ntohl(flowData.flowY);
           	         flowData.extLibDataSize=    ntohl(flowData.extLibDataSize);
           	         flowData.userPriviFlags    =oapc_util_ntohll(flowData.userPriviFlags);
           	         flowData.dataFlowFlags     =ntohl(flowData.dataFlowFlags);
           	         flowData.flowFlags         =ntohl(flowData.flowFlags);
           	         flowData.type              =ntohl(flowData.type);
           	         flowData.cycleMicros       =ntohl(flowData.cycleMicros);
           	         utf16BEToASCII(flowData.store_libname,MAX_LIBNAME_LENGTH);
                     utf16BEToASCII(flowData.store_name,MAX_NAME_LENGTH);
           	         //wxString libname=g_charToStringUTF16BE(flowData.store_libname,sizeof(flowData.store_libname));
           	         if (((m_devName) && (strlen(flowData.store_libname)>0) && (!strcmp(m_devName,flowData.store_libname))) ||
           	             ((m_devID>0) && (m_devID==flowData.flowData.id)))
                     {   	            
           	             char *c;
           	             
           	             snprintf(m_inNodeName,MAX_NODENAME_LENGTH+4,"/%s/in",flowData.store_name);
                         snprintf(m_outNodeName,MAX_NODENAME_LENGTH+4,"/%s/out",flowData.store_name);
#ifdef ENV_LINUX
         	            c=strstr(flowData.store_libname,".dll");
         	            if (c)
         	            {
         	               *c=0;
         	               strcat(flowData.store_libname,".so");
         	            }
         	            c=strstr(flowData.store_libname,".DLL");
         	            if (c)
         	            {
         	               *c=0;
         	               strcat(flowData.store_libname,".so");
         	            }
         	            c=strstr(flowData.store_libname,"\\");
         	            while (c)
         	            {
         	            	*c='/';
             	            c=strstr(flowData.store_libname,"\\");
         	            };
#endif
#ifdef ENV_WINDOWS
         	            c=strstr(flowData.store_libname,".so");
         	            if (c)
         	            {
         	               *c=0;
         	               strcat(flowData.store_libname,".dll");
         	            }
         	            c=strstr(flowData.store_libname,"/");
         	            while (c)
         	            {
         	            	*c='\\';
             	            c=strstr(flowData.store_libname,"/");
         	            };
#endif
         	            sprintf(libPath,"%s%s",LIB_PATH,flowData.store_libname);
         	            dynLib=oapc_dlib_load(libPath);
#ifdef ENV_LINUX
                        if (!dynLib) printf("oapc_dlib_load-error: %s\n",dlerror());
#endif
         	            if (dynLib)
         	            {
         	               bool ok;
         	               
         	               fn_oapc_get_capabilities=(lib_oapc_get_capabilities)oapc_dlib_get_symbol(dynLib,"oapc_get_capabilities");
         	               fn_oapc_get_output_flags=(lib_oapc_get_output_flags)oapc_dlib_get_symbol(dynLib,"oapc_get_output_flags");
         	               fn_oapc_set_loaded_data= (lib_oapc_set_loaded_data) oapc_dlib_get_symbol(dynLib,"oapc_set_loaded_data");
         	               fn_oapc_create_instance2=(lib_oapc_create_instance2)oapc_dlib_get_symbol(dynLib,"oapc_create_instance2");
                           fn_oapc_delete_instance= (lib_oapc_delete_instance) oapc_dlib_get_symbol(dynLib,"oapc_delete_instance");
         	               fn_oapc_init=            (lib_oapc_init)            oapc_dlib_get_symbol(dynLib,"oapc_init");
         	               fn_oapc_exit=            (lib_oapc_exit)            oapc_dlib_get_symbol(dynLib,"oapc_exit");
         	               fn_oapc_set_io_callback= (lib_oapc_set_io_callback) oapc_dlib_get_symbol(dynLib,"oapc_set_io_callback");
         	               fn_oapc_set_digi_value=  (lib_oapc_set_digi_value)  oapc_dlib_get_symbol(dynLib,"oapc_set_digi_value");
                           fn_oapc_get_digi_value=  (lib_oapc_get_digi_value)  oapc_dlib_get_symbol(dynLib,"oapc_get_digi_value");
         	               fn_oapc_set_num_value=   (lib_oapc_set_num_value)   oapc_dlib_get_symbol(dynLib,"oapc_set_num_value");
                           fn_oapc_get_num_value=   (lib_oapc_get_num_value)   oapc_dlib_get_symbol(dynLib,"oapc_get_num_value");
         	               fn_oapc_set_char_value=  (lib_oapc_set_char_value)  oapc_dlib_get_symbol(dynLib,"oapc_set_char_value");
         	               fn_oapc_get_char_value=  (lib_oapc_get_char_value)  oapc_dlib_get_symbol(dynLib,"oapc_get_char_value");
         	               fn_oapc_set_bin_value=   (lib_oapc_set_bin_value)   oapc_dlib_get_symbol(dynLib,"oapc_set_bin_value");
         	               fn_oapc_get_bin_value=   (lib_oapc_get_bin_value)   oapc_dlib_get_symbol(dynLib,"oapc_get_bin_value");
         	               fn_oapc_release_bin_data=(lib_oapc_release_bin_data)oapc_dlib_get_symbol(dynLib,"oapc_release_bin_data");
         	               if (fn_oapc_create_instance2)
         	               {
         	            	   m_pluginData=fn_oapc_create_instance2(OAPC_INSTANCE_OPERATION);
         	            	   if (!m_pluginData)
         	            	   {
                	              gMessage("Instantiation of Plug-In failed!");
                	              fclose(FHandle);
                	              return false;         	            		   
         	            	   }
         	               }
         	               else
         	               {
           	                  gMessage("Plug-In is invalid, function oapc_create_instance2() is missing!");
            	               fclose(FHandle);
            	               return false;
         	               }
                           subChunkSize-=loadEXIO(FHandle,&flowData,&ok);
                           if (!ok)
                           {
             	               fclose(FHandle);
             	               return false;
                           }
         	            }
         	            else
         	            {
         	            	gMessage("Could not load Plug-In %s!",libPath);
         	            	fclose(FHandle);
         	            	return false;
         	            }
           	         }
           	      }
               fseek(FHandle,subChunkSize,SEEK_CUR);
           	   }
            }
         fseek(FHandle,chunkSize,SEEK_CUR);
         }
      }
      fclose(FHandle);
   }
   else
   {
      printf("ERROR: File %s not found!",file);
      return false;
   }
   return true;
}



/**
 * Tries to (re)establish a connection to the Interlock Server
 * param[in] tries in case the connection could not be established immediately this parameter
 *           specifies how often the function should try to open a connection to the server;
 *           this value has to be greater than 0 (it is named tries, not retries!)
 * param[in] sleep this parameter is used only in case the first connection attempt failed, it
 *           specifies how many milliseconds the function may sleep before it has to try again;
 *           so the maximum execution time of this function is the number of tries multiplied
 *           with the sleep time
 * return true in case the connection could be established successfully, false in case an error
 *           occured
 */
static bool connectToIS(int tries,int sleep)
{
   char *isIP=NULL;
   int   i;

   if ((projectData.flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_DOUBLE_REMOTE) tries*=2; // in case of two remote servers ensure that all two IPs are tried the same number of times
   for (i=1; i<=tries; i++)
   {
      m_isHandle=oapc_ispace_get_instance();
      if (m_isHandle)
      {
         if (oapc_ispace_set_recv_callback(m_isHandle,&recv_callback)==OAPC_OK)        // make the callback-function known to the library
         {
            if (((projectData.flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_SINGLE_REMOTE) ||
                ((projectData.flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_DOUBLE_REMOTE))
            {
               if ((projectData.flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_DOUBLE_REMOTE)
               {
                  if (i%2==1) isIP=projectData.m_remSingleIP;
                  else  isIP=projectData.m_remDoubleIP;
                  if (verbose) printf("Trying custom IP %s for connection\n",isIP);
               }
               else
               {
                  isIP=projectData.m_remSingleIP;
                  if (verbose) printf("Using custom IP %s for connection\n",isIP);
               }
            }
            if (oapc_ispace_connect(m_isHandle,isIP,0,NULL)==OAPC_OK)
            {
               return true;
            }
            else if (i==tries) gMessage("Interlock Server connection error!");
         }
         else if (i==tries) gMessage("Interlock Server callback registration failed!");
      }
      else if (i==tries) gMessage("Interlock Server instance error!\n");
      oapc_thread_sleep(sleep);
   }
   return false;
}



/**
 * This is the callback function that may be used by the plug-in to inform about data that are
 * available for reading with oapc_get_xxx_value(), it is used only in case the plug-in supports
 * such a callback, elsewhere it is polled out of the main function
 */
static void plugin_io_callback(unsigned long output,unsigned long callbackID)
{
   int                   rc=OAPC_OK,pos;

   if (verbose) printf("Fetching data from plug-in output #%ld\n",output);
#ifdef _DEBUG
   writeLog("Fetching data from plug-in output #%ld\n",output);
#endif
   switch (output)
   {
      case OAPC_DIGI_IO0:
      case OAPC_NUM_IO0:
      case OAPC_CHAR_IO0:
      case OAPC_BIN_IO0:
         pos=0;
         break;
      case OAPC_DIGI_IO1:
      case OAPC_NUM_IO1:
      case OAPC_CHAR_IO1:
      case OAPC_BIN_IO1:
         pos=1;
         break;
      case OAPC_DIGI_IO2:
      case OAPC_NUM_IO2:
      case OAPC_CHAR_IO2:
      case OAPC_BIN_IO2:
         pos=2;
         break;
      case OAPC_DIGI_IO3:
      case OAPC_NUM_IO3:
      case OAPC_CHAR_IO3:
      case OAPC_BIN_IO3:
         pos=3;
         break;
      case OAPC_DIGI_IO4:
      case OAPC_NUM_IO4:
      case OAPC_CHAR_IO4:
      case OAPC_BIN_IO4:
         pos=4;
         break;
      case OAPC_DIGI_IO5:
      case OAPC_NUM_IO5:
      case OAPC_CHAR_IO5:
      case OAPC_BIN_IO5:
         pos=5;
	      break;
      case OAPC_DIGI_IO6:
      case OAPC_NUM_IO6:
      case OAPC_CHAR_IO6:
      case OAPC_BIN_IO6:
         pos=6;
         break;
      case OAPC_DIGI_IO7:
      case OAPC_NUM_IO7:
      case OAPC_CHAR_IO7:
      case OAPC_BIN_IO7:
         pos=7;
         break;
      default:
         pos=0;
         assert(0);
         break;
   }
   if (output & OAPC_DIGI_IO_MASK)
   {
      if (fn_oapc_get_digi_value)
      {
         struct oapc_digi_value_block digiValue;

         if (fn_oapc_get_digi_value(m_pluginData,pos,&digiValue.digiValue)==OAPC_OK)
         {
            rc=oapc_ispace_set_value(m_isHandle,m_outNodeName,output,&digiValue,NULL);
            messageSent=true;
         }
	   }
	   else
	   {
	      // flags but no function -> warning?
	   }
	}
	else if (output & OAPC_NUM_IO_MASK)
   {
      if (fn_oapc_get_num_value)
      {
         double                      num;
         struct oapc_num_value_block numValue;

         if (fn_oapc_get_num_value(m_pluginData,pos,&num)==OAPC_OK)
         {
            oapc_util_dbl_to_block(num,&numValue);
            rc=oapc_ispace_set_value(m_isHandle,m_outNodeName,output,&numValue,NULL);
            messageSent=true;
         }
      }
      else
      {
         // flags but no function -> warning?
      }
   }
   else if (output & OAPC_CHAR_IO_MASK)
   {
      if (fn_oapc_get_char_value)
      {
         struct oapc_char_value_block charValue;

         if (fn_oapc_get_char_value(m_pluginData,pos,MAX_TEXT_LEN,charValue.charValue)==OAPC_OK)
         {
            rc=oapc_ispace_set_value(m_isHandle,m_outNodeName,output,&charValue,NULL);
            messageSent=true;
         }
      }
      else
      {
         // flags but no function -> warning?
      }
   }
   else if (output & OAPC_BIN_IO_MASK)
   {
      if ((fn_oapc_get_bin_value) && (fn_oapc_release_bin_data))
      {
         struct oapc_bin_head *bin=NULL;

         if (fn_oapc_get_bin_value(m_pluginData,pos,&bin)==OAPC_OK)
         {
            rc=oapc_ispace_set_value(m_isHandle,m_outNodeName,output,bin,NULL);
            fn_oapc_release_bin_data(m_isHandle,output);
            messageSent=true;
         }
      }
      else
      {
         // flags but no function -> warning?
      }
   }
   if (rc!=OAPC_OK)
   {
      oapc_ispace_disconnect(m_isHandle);
      if (!connectToIS(3,50))
      {
         doExit=true;
         gMessage("Connection to Interlock Server lost!");
      }
   }
}



#ifdef ENV_WINDOWS
static int splitCmdLine(char *cCmdLine,char *argv[])
{
   int          ctr=0;
   char        *c,*cprev,*cstart;

   if (strlen(cCmdLine)<2) return 0;
   c=cCmdLine+1;
   cprev=cCmdLine;
   cstart=cCmdLine;
   while (*c!=0)
   {
      if ((*c==' ') && (*cprev!='\\'))
      {
         *c=0;
         argv[ctr]=cstart;
         ctr++;
         cstart=c+1;
      }
      c++;
      cprev++;
   }
   argv[ctr]=cstart;
   return ctr;
}



int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
   WSADATA   wsaData;
   int       argc;
   char     *argv[100];

   WSAStartup((MAKEWORD(1, 1)), &wsaData);
   argc=splitCmdLine(GetCommandLine(),argv);
#else
int main(int argc,char *argv[])
{
#endif
   float lastCycleTime,lastPingTime,currTime,diffTime,cycleTime;

   if (!getSwitches(argc,argv))
   {
      releaseAll();
      return 0;
   }

#ifdef _DEBUG
 #ifdef ENV_WINDOWS
   FHandle=fopen("C:\\tmp\\plugger.log","a");
 #else
   FHandle=fopen("/tmp/plugger.log","a");
 #endif
   writeLog("=======================================================");
   writeLog("starting plugger");
#endif
   if (verbose) printf("Connecting to Interlock Server...\n");
   recvMutex=oapc_thread_mutex_create();
   if ((recvMutex) && (connectToIS(10,400)))
   {
      if (verbose)
      {
         printf("Loading project...\n");
#ifdef _DEBUG
         writeLog("Loading project...");
#endif
      }
      if (!loadProject(m_file,m_playerWorkingDirectory))
      {
         releaseAll();
         return 0;
      }
      if (m_file) free(m_file);
      m_file=NULL;
      if (m_playerWorkingDirectory) free(m_playerWorkingDirectory);
      m_playerWorkingDirectory=NULL;
      if (!m_pluginData)
      {
         gMessage("Requested Plug-In could not be found in project file!");
         releaseAll();
         return 0;
      }

      if ((fn_oapc_get_capabilities) && (fn_oapc_get_capabilities() & OAPC_ACCEPTS_IO_CALLBACK))
      {
         if (fn_oapc_set_io_callback) // TODO: check capabilities     
          fn_oapc_set_io_callback(m_pluginData,plugin_io_callback,42);
      }
      else
      {
         if (!fn_oapc_get_output_flags)
         {
            gMessage("Plug-In corrupt, can't get information about IOs");
            releaseAll();
            return 0;
         }
         else m_outputFlags=fn_oapc_get_output_flags();
      }

      if (verbose)
      {
         printf("Entering poll loop...\n");
#ifdef _DEBUG
         writeLog("Entering poll loop...");
#endif
      }
      cycleTime=flowData.cycleMicros/1000.0F;
      lastCycleTime=(float)clock()/CLOCKS_PER_SEC;
      lastPingTime=(float)clock()/CLOCKS_PER_SEC;
      while (!doExit)
      {
         if (!recvList.empty())
         {
            std::list<struct recvDataBlock*>::iterator it;
            struct recvDataBlock                      *recvData;

#ifdef _DEBUG
            writeLog("found data in list");
#endif
            oapc_thread_mutex_lock(recvMutex);
            it=recvList.begin();
            while (it!=recvList.end())
            for ( it=recvList.begin() ; it !=recvList.end(); it++ )
            {
               recvData=*it;
#ifdef _DEBUG
               writeLog("preparing IO data 0x%X to hand over to plug-in",recvData->IOMask);
#endif
               if (recvData->IOMask==OAPC_DIGI_IO_MASK)      fn_oapc_set_digi_value(m_pluginData,recvData->IONum,recvData->d.digiValue);
               else if (recvData->IOMask==OAPC_NUM_IO_MASK)
               {
//                  printf("Plugger: sending %f\n",recvData->d.numValue);
                  fn_oapc_set_num_value(m_pluginData,recvData->IONum,recvData->d.numValue);
               }
               else if (recvData->IOMask==OAPC_CHAR_IO_MASK) fn_oapc_set_char_value(m_pluginData,recvData->IONum,recvData->d.charValue);
               else if (recvData->IOMask==OAPC_BIN_IO_MASK)
               {
                  fn_oapc_set_bin_value(m_pluginData,recvData->IONum,recvData->d.binValue);
                  free(recvData->d.binValue);
               }
#ifdef _DEBUG
               writeLog("IO data transmitted successfully");
#endif
               free(recvData);
               it=recvList.erase(it);
            }
            oapc_thread_mutex_unlock(recvMutex);
         }
         else oapc_thread_sleep(10);
         currTime=(float)clock()/CLOCKS_PER_SEC;
         diffTime=currTime-lastCycleTime;
         if ((m_outputFlags>0) && (diffTime>cycleTime))
         {
            int i;
            unsigned int bitmask=0x00000001;

            messageSent=false;
            for (i=0; i<MAX_NUM_IOS*4; i++)
            {
               if (m_outputFlags & bitmask) plugin_io_callback(m_outputFlags & bitmask,42);
               bitmask=bitmask<<1;
            }
            oapc_thread_sleep(flowData.cycleMicros/1000);
            lastCycleTime=currTime;
            if (messageSent) lastPingTime=currTime; // sending a message worth as much as a ping
         }
         else
         {
            diffTime=currTime-lastPingTime;
            if (diffTime>PING_DELAY_TIME)
            {
#ifdef _DEBUG
               writeLog("sending ping (%f %d %f %f)",diffTime,PING_DELAY_TIME,currTime,lastPingTime);
#endif
               if (oapc_ispace_set_value(m_isHandle,"/system/ping",0,NULL,NULL)!=OAPC_OK)
                connectToIS(1,100);
               lastPingTime=currTime;
            }
         }
      }
   }
   if (verbose) printf("...exiting\n");
#ifdef _DEBUG
   writeLog("...exiting");
   if (FHandle) fclose(FHandle);
#endif
   releaseAll();
   return 0;
}

