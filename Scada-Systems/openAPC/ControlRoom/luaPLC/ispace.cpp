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

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" 
{
#endif
   #include <lua.h>

   #include <lauxlib.h>
   #include <lualib.h>
#ifdef __cplusplus
}
#endif

#include "ispace.h"
#include "liboapc.h"
#include "oapc_libio.h"

       void  *m_luaMutex;

static void  *g_handler[2]={NULL,NULL};
static char   value_received=0,value_ok,value_bool;
static double value_number;
static char   value_string[MAX_TEXT_LEN+1];
static int    get_value_io=-1,get_value_type=0;


static void recv_callback(void *handler,char *nodeName,unsigned int cmd,unsigned int ios,void *values[MAX_NUM_IOS])
{
   int                           n;
   unsigned int                  bits=0x00010101;
   struct oapc_digi_value_block *digiValue;
   struct oapc_num_value_block  *numValue;
   struct oapc_char_value_block *charValue;
   double                        d;

   oapc_thread_mutex_lock(m_luaMutex);
   //lua_getfield(globalL, LUA_GLOBALSINDEX, "oapc_ispace_recv_callback"); // the function to be called
   lua_getglobal(globalL,"oapc_ispace_recv_callback"); // should work with all 5.x

   lua_pushstring(globalL,nodeName);
   lua_pushnumber(globalL,cmd);
   lua_pushnumber(globalL,ios);
   for (n=0; n<8; n++)
   {
   	if (ios & bits & OAPC_DIGI_IO_MASK)
   	{
   		digiValue=(struct oapc_digi_value_block*)values[n];
   		lua_pushboolean(globalL,digiValue->digiValue);
   	}
   	else if (ios & bits & OAPC_NUM_IO_MASK)
   	{
   		numValue=(struct oapc_num_value_block*)values[n];
   		d=oapc_util_block_to_dbl(numValue);
   		lua_pushnumber(globalL,d);
   	}
   	else if (ios & bits & OAPC_CHAR_IO_MASK)
   	{
   		charValue=(struct oapc_char_value_block*)values[n];
   		lua_pushstring(globalL,charValue->charValue);
   	}
   	else 
   	{
   		lua_pushnil(globalL);
   	}
   	bits=bits<<1;
   }
   lua_pcall(globalL,11,0,0);
   oapc_thread_mutex_unlock(m_luaMutex);
}



/**
 * Callback-function for internal use, it fetches the node value returned by the get-value-function
 */
static void get_callback(void *handler,char *nodeName,unsigned int cmd,unsigned int ios,void *values[MAX_NUM_IOS])
{
	unsigned int                  bits=0x00010101;
	int                           i;
   struct oapc_digi_value_block *digiValue;
   struct oapc_num_value_block  *numValue;
   struct oapc_char_value_block *charValue;

	value_ok=0;
	if (get_value_io<0)
	{
		value_received=1;
		return;
	}
	if (get_value_io>0) for (i=0; i<get_value_io; i++) bits=bits<<1;
	
	if (ios & bits & OAPC_DIGI_IO_MASK)
	{
 		digiValue=(struct oapc_digi_value_block*)values[get_value_io];
  		value_bool=digiValue->digiValue;
		get_value_type=OAPC_DIGI_IO_MASK;
	}
	else if (ios & bits & OAPC_NUM_IO_MASK)
	{
  		numValue=(struct oapc_num_value_block*)values[get_value_io];
  		value_number=oapc_util_block_to_dbl(numValue);
		get_value_type=OAPC_NUM_IO_MASK;
	}
	else if (ios & bits & OAPC_CHAR_IO_MASK)
	{
  		charValue=(struct oapc_char_value_block*)values[get_value_io];
   	strncpy(value_string,charValue->charValue,MAX_TEXT_LEN);
		get_value_type=OAPC_CHAR_IO_MASK;
	}
	else
	{
		value_received=1;
		return;
	}
	
	value_ok=1;
   value_received=1;
	return;
}



int lua_ispace_connect(lua_State *L)
{
   int            n,ret,port,i;
   char           host[101];
   
   n = lua_gettop(L);    /* number of arguments */
   if (n!=2)
   {
      lua_pushstring(L, "incorrect number of arguments");
      lua_error(L);
   }
   if (!lua_isstring(L,1)) 
   {
      lua_pushstring(L, "incorrect argument type");
      lua_error(L);
   }
   strncpy(host,lua_tostring(L,1),100);
   if (!lua_isnumber(L,2)) 
   {
      lua_pushstring(L, "incorrect argument type");
      lua_error(L);
   }
   port=(int)lua_tonumber(L,2);
   if ((port<0) || (port>65535))
   {
      lua_pushstring(L, "argument out of valid range");
      lua_error(L);
   }
   
   for (i=0; i<2; i++)
   {
      g_handler[i]=oapc_ispace_get_instance();
      if (!g_handler[i])
      {
         lua_pushnumber(L,OAPC_ERROR_RESOURCE);         /* result */
         return 1;                   /* number of results */
      }
      if (i==0) ret=oapc_ispace_set_recv_callback(g_handler[0],&recv_callback);
      else ret=oapc_ispace_set_recv_callback(g_handler[1],&get_callback);
      if (ret!=OAPC_OK)
      {
         lua_pushnumber(L,ret);      /* result */
         return 1;                   /* number of results */
      }
      if (strlen(host)<1) ret=oapc_ispace_connect(g_handler[i],NULL,(unsigned short)port,NULL);
      else ret=oapc_ispace_connect(g_handler[i],host,(unsigned short)port,NULL);
      if ((i==0) && (ret!=OAPC_OK))
      {
         lua_pushnumber(L,ret);         /* result */
         return 1;                      /* number of results */
      }
   }
   lua_pushnumber(L,ret);         /* result */
   return 1;                      /* number of results */
}



int lua_ispace_set_data(lua_State *L)
{
   int                          n,ret,ios;
   char                         nodeName[101];
   unsigned int                 bits=0x00010101;
	void                        *values[MAX_NUM_IOS]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}; // the array for sending data
   struct oapc_digi_value_block digiValue[MAX_NUM_IOS];
   struct oapc_num_value_block  numValue[MAX_NUM_IOS];
   struct oapc_char_value_block charValue[MAX_NUM_IOS];
   double                       d;
   
   n = lua_gettop(L);    // number of arguments
   if (n!=10) 
   {
      lua_pushstring(L, "incorrect number of arguments");
      lua_error(L);
   }
   if (!lua_isstring(L,1)) 
   {
      lua_pushstring(L, "incorrect argument type");
      lua_error(L);
   }
   strncpy(nodeName,lua_tostring(L,1),100);
   if (!lua_isnumber(L,2)) 
   {
      lua_pushstring(L, "incorrect argument type");
      lua_error(L);
   }
   ios=(int)lua_tonumber(L,2);
   for (n=3; n<11; n++)
   {
   	if (ios & bits & OAPC_DIGI_IO_MASK)
   	{
         if (!lua_isboolean(L,n)) 
         {
            lua_pushstring(L, "incorrect argument type");
            lua_error(L);
         }
         digiValue[n-3].digiValue=lua_toboolean(L,n);
         values[n-3]=&digiValue[n-3];         
   	}
   	else if (ios & bits & OAPC_NUM_IO_MASK)
   	{
         if (!lua_isnumber(L,n)) 
         {
            lua_pushstring(L, "incorrect argument type");
            lua_error(L);
         }
         d=lua_tonumber(L,n);
         oapc_util_dbl_to_block(d,&numValue[n-3]);
         values[n-3]=&numValue[n-3];         
   	}
   	else if (ios & bits & OAPC_CHAR_IO_MASK)
   	{
         if (!lua_isstring(L,n)) 
         {
            lua_pushstring(L, "incorrect argument type");
            lua_error(L);
         }
         strncpy(charValue[n-3].charValue,lua_tostring(L,n),MAX_TEXT_LEN);
         values[n-3]=&charValue[n-3];         
   	}
   	else 
   	{
   		lua_topointer(L,n); // remove the unused argument form the stack???
   	}
   	bits=bits<<1;
   }
   ret=oapc_ispace_set_data(g_handler[0],nodeName,ios,values,NULL);
   lua_pushnumber(L,ret);
   return 1;                   /* number of results */
}



int lua_ispace_set_value(lua_State *L)
{
   char                         nodeName[101];
   int                          n,ioNum,ios=0,ret;
	void                        *values[MAX_NUM_IOS]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}; // the array for sending data
   struct oapc_digi_value_block digiValue;
   struct oapc_num_value_block  numValue;
   struct oapc_char_value_block charValue;
   double                       d;

   n = lua_gettop(L);    // number of arguments
   if (n!=2) 
   {
      lua_pushstring(L, "incorrect number of arguments");
      lua_error(L);
   }
   if (!lua_isstring(L,1)) 
   {
      lua_pushstring(L, "incorrect argument type");
      lua_error(L);
   }
   strncpy(nodeName,lua_tostring(L,1),100);
   if (strlen(nodeName)<6)
   {
      lua_pushstring(L, "incorrect node name value");
      lua_error(L);
   }
   ioNum=atoi(nodeName+strlen(nodeName)-1);
   nodeName[strlen(nodeName)-2]=0;

   if (lua_isboolean(L,n))
   {
      digiValue.digiValue=lua_toboolean(L,n);
      values[ioNum]=&digiValue;         
      ios=OAPC_DIGI_IO0;
   } 
   else if (lua_isnumber(L,2)) 
   {
      d=lua_tonumber(L,n);
      oapc_util_dbl_to_block(d,&numValue);
      values[ioNum]=&numValue;         
      ios=OAPC_NUM_IO0;
   }
   else if (lua_isstring(L,2)) 
   {
      strncpy(charValue.charValue,lua_tostring(L,n),MAX_TEXT_LEN);
      values[ioNum]=&charValue;         
      ios=OAPC_CHAR_IO0;
   }
   else
   {
      lua_pushstring(L, "incorrect argument type");
      lua_error(L);
   }
   while (ioNum>0)
   {
   	ios=ios<<1;
   	ioNum--;
   }
   ret=oapc_ispace_set_data(g_handler[0],nodeName,ios,values,NULL);
   lua_pushnumber(L,ret);      // result 
   return 1;                   /* number of results */
}



int lua_ispace_get_value(lua_State *L)
{
   int                          n;
   char                         nodeName[101];
   
   n = lua_gettop(L);    // number of arguments
   if (n!=1) 
   {
      lua_pushstring(L, "incorrect number of arguments");
      lua_error(L);
   }
   if (!lua_isstring(L,1)) 
   {
      lua_pushstring(L, "incorrect argument type");
      lua_error(L);
   }
   strncpy(nodeName,lua_tostring(L,1),100);
   if (strlen(nodeName)<6)
   {
      lua_pushstring(L, "incorrect node name value");
      lua_error(L);
   }
   value_received=0;
   get_value_io=atoi(nodeName+strlen(nodeName)-1);
   if ((get_value_io<0) || (get_value_io>=MAX_NUM_IOS))
   {
      lua_pushstring(L, "incorrect node name IO number");
      lua_error(L);
   } 
   nodeName[strlen(nodeName)-2]=0;

   oapc_thread_mutex_unlock(m_luaMutex);
   (void)oapc_ispace_request_data(g_handler[1],nodeName,NULL);
   for (n=0; n<200; n++) 
   {
   	if (value_received) break;
   	oapc_thread_sleep(10);
   }
   get_value_io=-1;
   oapc_thread_mutex_lock(m_luaMutex);
   if ((value_received) && (value_ok))
   {
      lua_pushnumber(L,OAPC_OK);      // result */
      if (get_value_type==OAPC_DIGI_IO_MASK) lua_pushboolean(L,value_bool);
      else if (get_value_type==OAPC_NUM_IO_MASK) lua_pushnumber(L,value_number);
      else if (get_value_type==OAPC_CHAR_IO_MASK) lua_pushstring(L,value_string);
   }
   else
   {
      lua_pushnumber(L,OAPC_ERROR);      // result */
      lua_pushnil(L);
   }   
   return 2;
}


int lua_ispace_request_data(lua_State *L)
{
   int                          n,ret;
   char                         nodeName[101];
   
   n = lua_gettop(L);    // number of arguments
   if (n!=1) 
   {
      lua_pushstring(L, "incorrect number of arguments");
      lua_error(L);
   }
   if (!lua_isstring(L,1)) 
   {
      lua_pushstring(L, "incorrect argument type");
      lua_error(L);
   }
   strncpy(nodeName,lua_tostring(L,1),100);
   ret=oapc_ispace_request_data(g_handler[0],nodeName,NULL);
   lua_pushnumber(L,ret);      // result
   return 1;                   /* number of results */
}



int lua_ispace_request_all_data(lua_State *L)
{
   int                          n,ret;
   
   n = lua_gettop(L);    // number of arguments
   if (n!=0) 
   {
      lua_pushstring(L, "incorrect number of arguments");
      lua_error(L);
   }
   ret=oapc_ispace_request_all_data(g_handler[0],NULL);
   lua_pushnumber(L,ret);      // result
   return 1;                   /* number of results */
}



int lua_ispace_disconnect(lua_State *L)
{
   int                          n,ret;
   
   n = lua_gettop(L);    /* number of arguments */
   if (n!=0) 
   {
      lua_pushstring(L, "incorrect number of arguments");
      lua_error(L);
   }
   ret=oapc_ispace_disconnect(g_handler);
   lua_pushnumber(L,ret);         /* result */
   return 1;                   /* number of results */
}



