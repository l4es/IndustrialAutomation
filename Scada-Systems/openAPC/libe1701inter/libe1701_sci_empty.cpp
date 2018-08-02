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

#ifdef ENV_WINDOWS // SCI interface is Windows-only

#include "libe1701inter.h"

#undef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__        ( 449 )

// these header files can be found in SAMLight-demo-package, it can be downloaded
// for free at scaps.com, no license is required for usage
#include "sc_errorcodes.h"
#include "sc_optic_type_lib.h"

E1701_API long ScSCISetContinuousMode(long Mode)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetContinuousMode(long *Mode)
{
    return SC_ERROR;
}

E1701_API long ScSCIDevicePixelLine(float *PixelLine, long PixelCount, double dx, double dy, double dz)
{
    return SC_ERROR;
}

E1701_API long ScSCIRasterPixelLine(float *PixelLine, long PixelCount, double PixelPeriod)
{
    return SC_ERROR;
}

E1701_API long ScSCIRasterStart(double origin_x, double origin_y, double dir_x, double dir_y, double pixel_step, double line_step,double acceleration_time, double deceleration_time, double line_offset,double jump_speed, double wait_delay_0, double limit_value, long mode_flags)
{
    return SC_ERROR;
}

E1701_API long ScSCIRasterEnd()
{
    return SC_ERROR;
}

E1701_API long ScSCIGetDeviceName(long NameId,char *Name)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetDeviceCaps(long CapsId,long *Caps)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetDeviceData(long DataId, long DataLength, unsigned char *Data)
{
    return SC_ERROR;
}

E1701_API long ScSCISetDeviceData(long DataId, long DataLength, unsigned char *Data)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetExternalTrigger(long *Value)
{
    return SC_ERROR;
}

E1701_API long ScSCISetExternalTriggerCount(long Count)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetExternalTriggerCount(long *Count)
{
    return SC_ERROR;
}

E1701_API long ScSCISetEnableHead(long Value)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetEnableHead(long *Value)
{
    return SC_ERROR;
}

E1701_API long ScSCISetZField(double ZMin, double ZMax)
{
    return SC_ERROR;
}


E1701_API long ScSCIGetZField(double *ZMin, double *ZMax)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetZGain(double *GainZ)
{
    return SC_ERROR;
}

E1701_API long ScSCISetZGain(double GainZ)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetHomePosition(double *HomeX, double *HomeY)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetZHomePosition(double *HomeZ)
{
    return SC_ERROR;
}

E1701_API long ScSCISetHomePosition(double HomeX, double HomeY)
{
    return SC_ERROR;
}

E1701_API long ScSCISetZHomePosition(double HomeZ)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetZOffset(double *OffsetZ)
{
    return SC_ERROR;
}

E1701_API long ScSCISetZOffset(double OffsetZ)
{
    return SC_ERROR;
}

E1701_API long ScSCISetZWorkingArea(double ZMin, double ZMax)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetZWorkingArea(double *ZMin, double *ZMax)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetHomeJump(long *Value)
{
    return SC_ERROR;
}

E1701_API long ScSCISetHomeJump(long Value)
{
    return SC_ERROR;
}


E1701_API long ScSCIMaxExternalTriggerCount(long Count)
{
    return SC_ERROR;
}

E1701_API long ScSCIResetExternalTriggerCount(void)
{
    return SC_ERROR;
}


E1701_API long ScSCISetDeviceEnableFlags(long FlagsId, long Flags)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetDeviceEnableFlags(long FlagsId, long *Flags)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetDevicePath(long PathId,char *path)
{
    return SC_ERROR;
}

E1701_API long ScSCISetDeviceMiscValueD(long MiscId,double Value)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetDeviceMiscValueD(long MiscId,double *Value)
{
    return SC_ERROR;
}

E1701_API long ScSCISetHeadCount(unsigned long Count)
{
    return SC_ERROR;
}

E1701_API long ScSCIStreamInfo(long ident,long i_1,long i_2)
{
    return SC_ERROR;
}


E1701_API long ScSCIGetSpeed(double *Speed)
{
    return SC_ERROR;
}

E1701_API long ScSCISetSpeed(double Speed)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetStyleSet(unsigned long *Style)
{
    return SC_ERROR;
}

E1701_API long ScSCISetStyleSet(unsigned long Style)
{
    return SC_ERROR;
}

E1701_API long ScSCISetLoopMode(long Mode)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetLoopMode(long *Mode)
{
    return SC_ERROR;
}

E1701_API long ScSCISetLoop(long Count)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetLoop(long *Count)
{
    return SC_ERROR;
}

E1701_API long ScSCISetMessageWindow(long hwnd)
{
    return SC_ERROR;
}

E1701_API long ScSCISetAxisState(long state)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetAxisState(long *state)
{
    return SC_ERROR;
}

E1701_API long ScSCISaveSettings(const char *FileName)
{
    return SC_ERROR;
}

E1701_API long ScSCILoadSettings(const char *FileName)
{
    return SC_ERROR;
}

E1701_API long ScSCIEditSettings(void)
{
    return SC_ERROR;
}

E1701_API long ScSCIUpdateDeviceStyle(void)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetInterfaceVersion(unsigned long *Version) 
{
    return SC_ERROR;
}

E1701_API long ScSCIGetDebugMode(unsigned long *Flags)
{
    return SC_ERROR;
}

E1701_API long ScSCISetDebugMode(unsigned long Flags)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetIdentString(char *Ident)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetDeviceMapLaserPort(long *Port)
{
    return SC_ERROR;
}

E1701_API long ScSCISetDeviceMapLaserPort(long Port)
{
    return SC_ERROR;
}

E1701_API long ScSCIGetUSCInfoLong(unsigned long offset, long *value)
{
    return SC_ERROR;
}

#endif //ENV_WINDOWS - SCI interface is Windows-only
