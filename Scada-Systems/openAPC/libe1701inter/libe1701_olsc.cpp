/******************************************************************************

This file is part of E1701inter shared library

E1701inter is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

E1701inter is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
E1701inter. If not, see <http://www.gnu.org/licenses/>.

E1701inter linking exception: Linking this library dynamically with other
modules is making a combined work based on this library. Thus, the terms and
conditions of the GNU General Public License cover the whole combination. As a
special exception, the copyright holders of this library give you permission
to link this library with independent modules to produce an executable,
regardless of the license terms of these independent modules, and to copy and
distribute the resulting executable under terms of your choice, provided that
you also meet, for each linked independent module, the terms and conditions of
the license of that module. An independent module is a module which is not
derived from or based on this library. If you modify this library, you may
extend this exception to your version of the library, but you are not
obliged to do so. If you do not wish to do so, delete this exception statement
from your version.

*******************************************************************************/

/*
This file partially bases on OpenLaserShowController.h which is (c) by 2009
Chris Favreau
*/

#include <stddef.h>
#include <string.h>

#include "libe1701.h"
#include "libe1701inter.h"

static struct LASER_SHOW_DEVICE_POINT m_lastPoint[E1701_MAX_HEAD_NUM+1]={{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}};

extern char m_isOpened[E1701_MAX_HEAD_NUM+1];
extern bool checkOpen(const unsigned char card);

// OLSC_GetAPIVersion
// Inputs: None
// Outputs: Returns a single integer that represents the version number
// Description: Returns the version of the OpenDAC API
//				so we don't make any critical errors when using this DLL
E1701_API int  OLSC_GetAPIVersion(void)
{
   return 2;
}

// OLSC_GetAPIVersion
// Inputs: Pointer to a string at least 64 characters in length
// Outputs: Returns success or failure
// Description: Returns the string name of the OpenDC Interface
E1701_API int  OLSC_GetInterfaceName(char *pString)
{
   strcpy(pString,"E1701A");
   return OLSC_ERROR_SUCCESS;
}

// OLSC_Initialize
// Inputs: None
// Outputs: Returns the number of available devices.
// Description: Initializes the hardware
E1701_API int  OLSC_Initialize(void)
{
   return 1;
}


// OLSC_Shutdown
// Inputs: None
// Outputs: Returns success or failure
// Description: Shuts down all devices
E1701_API int  OLSC_Shutdown(void)
{
   unsigned char i;

   for (i=1; i<=E1701_MAX_HEAD_NUM; i++)
   {
      m_isOpened[i]=0;
      E1701_close(i);
   }
   return OLSC_ERROR_SUCCESS;
}


// OLSC_GetDeviceCount
// Inputs: None
// Outputs: Returns the number of available devices
// Description: Has to be called AFTER Intialize has been called.  Thsi function
// is to be used to query the number of available devices without calling
// intialize again and again
E1701_API int  OLSC_GetDeviceCount(void)
{
   return 1; // there is no device detection available at the moment
}


// OLSC_GetCapabilities
// Inputs:  device Number, device capabilties structure to be filled in
// Outputs: Returns success or failure
// Description: Gets the capabilties of each device attached
//				Caps:
//				Device Name
//				Device Version
//				Min Speed/Max Speed
//				Min Frame Size/Max Frame Size
//				DMX In/Out?
//				TTL In/Out?
//				X/Y Resolution (8 to 16 bits)
//				Color Resolution (1 to 16 bits)
//				Uses callbacks?
//				Some reserved space for future use
E1701_API int  OLSC_GetDeviceCapabilities(int device_number, struct LASER_SHOW_DEVICE_CAPABILITIES &device_capabilities)
{
   if (!checkOpen(device_number)) return OLSC_ERROR_FAILED;
   strncpy(device_capabilities.name,"E1701A",1024);
   E1701_get_version(device_number,(unsigned short*)&device_capabilities.version_major,(unsigned short*)&device_capabilities.version_minor);
   device_capabilities.min_speed=100000;      device_capabilities.max_speed=100000;
   device_capabilities.min_frame_size=100000; device_capabilities.max_frame_size=100000;
   device_capabilities.has_dmx_in=false;      device_capabilities.has_dmx_out=false;
   if (E1701_get_card_info(device_number) & E1701_BSTATE_DIGI_EB)
   {
      device_capabilities.has_ttl_in=true;      device_capabilities.has_ttl_out=true; // Digi I/O board is available
   }
   else
   {
      device_capabilities.has_ttl_in=false;      device_capabilities.has_ttl_out=false; // no Digi I/O board is available
   }
   device_capabilities.xy_resolution=16;
   device_capabilities.color_resolution=16; // E1701A interface expects 16 bit numbers independent fro mreal hardware resolution
   device_capabilities.uses_callbacks=false;   
   return OLSC_ERROR_SUCCESS;
}


// OLSC_GetLastErrorNumber
// Inputs: device number, error string pointer, and string length max.
// Outputs: error number, and actual error string
// Description: Get the string and/or number associated with the last error
E1701_API int  OLSC_GetLastErrorNumber(int device_number, int &number, char *string_pointer, int string_length)
{
   return OLSC_ERROR_NONE;
}


// OLSC_SetConfigFile
// Inputs: configuration path string (expected to have leading '/')
// Outputs: Returns success or failure
// Description: Sets the path name to the configuration file for this OLSC interface
// Other supporting files such as libraries and other configuration files must be at this location too
E1701_API int  OLSC_SetConfigFile(char *pFullPathString)									// NEW IN V2
{
   return OLSC_ERROR_NONE;
}


// OLSC_ShowConfigWindow
// Inputs: none
// Outputs: success or failure
// Description:  Calling this function tells the OLSC interface to show its built in configuration window if it has one
// a return of failure indicates that no window is available or could not be opened.  Configuration
// windows are expected to NOT be blocking (Modal).
E1701_API int  OLSC_ShowConfigWindow(void)												// NEW IN V2
{
   return OLSC_ERROR_NONE;
}


// OLSC_Play
// Inputs: device number
// Outputs: Returns success or failure
// Description: Starts the output on a particular device or all devices
E1701_API int  OLSC_Play(int device_number)
{
   E1701_execute(device_number);
   return OLSC_ERROR_SUCCESS;
}


// OLSC_Pause
// Inputs: device number
// Outputs: Returns success or failure
// Description: Stops the output on a particular device or all devices
E1701_API int  OLSC_Pause(int device_number)
{
   E1701_stop_execution(device_number);
   return OLSC_ERROR_SUCCESS;
}


// OLSC_Shutter
// Inputs: device number, shutter state (LASER_SHOW_DEVICE_SHUTTER_STATE)
// Outputs: Returns success or failure
// Description: Turns the laser show device's shutter On/Off
E1701_API int  OLSC_Shutter(int device_number, int state)
{
   return OLSC_ERROR_NONE;
}


// OLSC_WriteFrame
// Inputs: device number, frame structure (LASER_SHOW_DEIVCE_FRAME)
// Outputs: Returns success or failure
// Description:
// Writes a frame to a particular device or all devices
// Pass in a frame:
// Point Count
// Display Speed PPS
// Point Array:
// X 16 bit unsigned
// Y 16 bit unsigned
// R 16 bit unsigned
// G 16 bit unsigned
// B 16 bit unsigned
// I 16 bit unsigned
//
// ** Any frame that is written will be displayed until another frame is written to take its place or the Output is Paused
E1701_API int  OLSC_WriteFrame(int device_number, struct LASER_SHOW_DEVICE_FRAME frame)
{
   int j;

   if (!checkOpen(device_number)) return OLSC_ERROR_FAILED;
   for (j=0; j<frame.point_count; j++)
   {
      OLSC_WritePoint(device_number,frame.points[j]);
   }
   return OLSC_ERROR_SUCCESS;
}


// OLSC_WritePoint
// Inputs: device number, point
// Outputs: success or failure
// Description: Stores a point in a buffer to be written later
E1701_API int  OLSC_WritePoint(int device_number, struct LASER_SHOW_DEVICE_POINT point)	// NEW IN V2
{
   E1701_set_pos(device_number,point.x<<10,point.y<<10,0,1);
   if ((point.r!=m_lastPoint[device_number].r) || (point.g!=m_lastPoint[device_number].g) ||
       (point.b!=m_lastPoint[device_number].b))
   {
      E1701_ana_a123(device_number,point.r,point.g,point.b);
      m_lastPoint[device_number]=point;
   }
   return OLSC_ERROR_SUCCESS;
}


// OLSC_WriteBufferedFrame
// Inputs: device number
// Outputs: success or failure
// Description: Writes all of the points that have been buffered up to this point
E1701_API int  OLSC_WriteBufferedFrame(int device_number)									// NEW IN V2
{
   return OLSC_ERROR_SUCCESS;
}

// OLSC_GetStatus()
// Inputs: device number, status DWORD
// Outputs:  Returns success or failure
// Description:
// Gets the status of a particular device
// Can be used for polling or in conjunction with Windows Message Callback
// Status Structure or Bit Mask:
// Bit 0 = Buffer Full
// Bit 1 = Buffer Empty
// Bit 2 = DMX Out Complete
// Bit 3 = DMX In Ready
//
// Bit 4 = TTL Out Complete
// Bit 5 = TLL In Ready
// Bit 6 = Reserved
// Bit 7 = Reserved
//
// Bit 8 = Reserved
// Bit 9-30 = Reserved
// Bit 31 = An Error Occured
E1701_API int  OLSC_GetStatus(int device_number, int &status)
{
   if (!checkOpen(device_number)) return OLSC_ERROR_FAILED;
   status=0;
   if (E1701_get_free_space(device_number,E1701_FREE_SPACE_PRIMARY)<900000) status|=0x01;
   status|=0x30; // TTL's are always ready and complete
   return OLSC_ERROR_SUCCESS;
}


// OLSC_SetCallback()
// Inputs: device number, parent window handle (for SendMessage), message DWORD
// Outputs:  Returns success or failure
// Description:
// Sets the call back window parent handle and windows message to send to run the callback
// and set the call back notification mask bits per the GetStatus mask.  The Callback message
// will return the Device Number in the wParam and the Status mask in the lParam.
// We all work in windows right?
// This interface is optional in the DLL and in the application.  The DLL writers should take this into account.
// So if you use the callback ... be able to function without it... it is used for efficiency.
E1701_API int  OLSC_SetCallback(int device_number, POLSCCALLBACK pCallback)				// NEW IN V2
{
   return OLSC_ERROR_NONE;
}


// OLSC_WriteDMX()
// Inputs: device number, start address, data pointer, data length
// Outputs:  Returns success or failure
// Description:
// Writes DMX data to a particular device
E1701_API int  OLSC_WriteDMX(int device_number, int start_address, unsigned char *data_pointer, int length)
{
   return OLSC_ERROR_FAILED;
}


// OLSC_ReadDMX()
// Inputs: device number, start address, data pointer, data length
// Outputs:  Returns success or failure
// Description:
// Reads DMX data from a particular device
E1701_API int  OLSC_ReadDMX(int device_number, int start_address, unsigned char *data_pointer, int length)
{
   return OLSC_ERROR_FAILED;
}


// OLSC_WriteTTL()
// Inputs: device number, data DWORD (up to 32 bits of TTL outputs)
// Outputs:  Returns success or failure
// Description:
// Writes TTL data from a particular device
E1701_API int  OLSC_WriteTTL(int device_number, int data)
{
   if (!checkOpen(device_number)) return OLSC_ERROR_FAILED;
   if (E1701_digi_write2(device_number,E1701_COMMAND_FLAG_STREAM,data,0xFFFFFFFF)==E1701_OK) return OLSC_ERROR_SUCCESS;
   return OLSC_ERROR_FAILED;
}


// OLSC_ReadTTL()
// Inputs: device number, data DWORD (up to 32 bits of TTL inputs)
// Outputs:  Returns success or failure
// Description:
// Reads TTL data from a particular device
E1701_API int  OLSC_ReadTTL(int device_number, int &data)
{
   if (!checkOpen(device_number)) return OLSC_ERROR_FAILED;
   data=E1701_digi_read(device_number);
   return OLSC_ERROR_SUCCESS;
}
