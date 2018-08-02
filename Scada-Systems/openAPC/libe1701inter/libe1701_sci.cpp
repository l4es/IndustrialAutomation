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

#include <stddef.h>
#include <assert.h>

#include "libe1701.h"
#include "libe1701inter.h"
#include "oapc_libio.h"

#undef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__        ( 449 )

// these header files can be found in SAMLight-demo-package, it can be downloaded
// for free at scaps.com, no license is required for usage
#include "sc_errorcodes.h"
#include "sc_optic_type_lib.h"

struct instData
{
   double        waXMin,waYMin,waXMax,waYMax;
   double        fXMin,fYMin,fXMax,fYMax;
   double        gainX,gainY,rot;
   double        offsetX,offsetY;
   double        fieldOffsetX,fieldOffsetY,factor;
   unsigned char laserState;
   long          standardDeviceOperationMode;
   double        halfPeriodStandBy,standByPulse,firstPulseLength,wobblePeriod,halfPeriodCO2,halfPeriod,laser1Length,qSwitchPeriod,qSwitchLength;
   long          miscWobbleAmp,miscEncoderKx,miscEncoderKy;
   double        markDelay,polyDelay,jumpDelay,longDelay,laserOnDelay,laserOffDelay;
   double        markSpeed,jumpSpeed;
   long          isExecuting;
   unsigned int  laserMode;     
};

static unsigned char   currHead=1;
static struct instData data[E1701_MAX_HEAD_NUM+1]={{-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0},
                                                   {-32768.0,-32768.0,32767.0,32767.0, -32768.0,-32768.0,32767.0,32767.0, 1.0,1.0,0.0, 0.0,0.0, 0.0,0.0,1.0, 0,0, 0.00005,0.000005,0.000015,0.0,0.00005,0.00005,0.000005,0.00005,0.000005, 0,0,0, 0.0,0.0,0.0,0.0,0.0,0.0, 100.0,1000.0, 0}};


E1701_API long ScSCIInit(void)
{
   return SC_OK;
}


E1701_API long ScSCIInitInterface(void)
{
   if (E1701_load_correction(currHead,NULL,0)==E1701_OK) return SC_OK;
   return SC_ERROR;
}


E1701_API long ScSCISetCardType(const char *CardName)
{
    return SC_OK;
}


E1701_API long ScSCIInitOptic(void)
{
    return SC_OK;
}


E1701_API long ScSCIGetHeadCount(unsigned long *Count)
{
   *Count=E1701_MAX_HEAD_NUM;
   return SC_OK;
}


E1701_API long ScSCIEnableHeadSelect(int Enable)
{
   if (!Enable) currHead=1;
   return SC_OK;
}


E1701_API long ScSCISetCurrentHead(long headID)
{
   if ((headID<0) || (headID>E1701_MAX_HEAD_NUM)) return SC_ERROR;
   currHead=(unsigned char)(headID);
   return SC_OK;
}


E1701_API long ScSCIGetCurrentHead(long *headID)
{
   *headID=currHead;
   return SC_OK;
}


E1701_API long ScSCISetDevicePath(long path_id,const char *path)
{
   if (path_id==scComStandardDeviceIDCorrectionFilePath0)
   {
      if (E1701_load_correction(currHead,path,0)==E1701_OK) return SC_OK;
   }
   return SC_ERROR;         
}


E1701_API long ScSCIGetHardwareState(long *state)
{
   *state=0;
   if ((E1701_get_card_state(currHead) & E1701_CSTATE_ERROR)==E1701_CSTATE_ERROR) *state=scComHardwareStateError;
   return SC_OK;
}


E1701_API long ScSCISetField(double XMin, double YMin, double XMax, double YMax)
{
   double fieldSizeX,fieldSizeY;

   data[currHead].fXMin=XMin; data[currHead].fYMin=YMin;
   data[currHead].fXMax=XMax; data[currHead].fYMax=YMax;

   fieldSizeX=XMax-XMin;
   fieldSizeY=YMax-YMin;
   data[currHead].fieldOffsetX=-XMin-(fieldSizeX/2);
   data[currHead].fieldOffsetY=YMin-(fieldSizeY/2);
   if (data[currHead].fieldOffsetX>data[currHead].fieldOffsetY) data[currHead].factor=65535.0/data[currHead].fieldOffsetX;
   else data[currHead].factor=65535.0/data[currHead].fieldOffsetY;

   return SC_OK;
}


E1701_API long ScSCIGetField(double *XMin, double *YMin, double *XMax, double *YMax)
{
   *XMin=data[currHead].fXMin; *YMin=data[currHead].fYMin;
   *XMax=data[currHead].fXMax; *YMax=data[currHead].fYMax;
   return SC_OK;
}


E1701_API long ScSCISetWorkingArea(double XMin, double YMin, double XMax, double YMax)
{
   data[currHead].waXMin=XMin; data[currHead].waYMin=YMin;
   data[currHead].waXMax=XMax; data[currHead].waYMax=YMax;
   return SC_OK;
}


E1701_API long ScSCIGetWorkingArea(double *XMin, double *YMin, double *XMax, double *YMax)
{
   *XMin=data[currHead].waXMin; *YMin=data[currHead].waYMin;
   *XMax=data[currHead].waXMax; *YMax=data[currHead].waYMax;
   return SC_OK;
}


E1701_API long ScSCISetGain(double GainX, double GainY)
{
   data[currHead].gainX=GainX; data[currHead].gainY=GainY;
   E1701_set_xy_correction(currHead,data[currHead].gainX,data[currHead].gainY,data[currHead].rot,0,0); // add offsetX/Y during wrapped jump and mark calls
   return SC_OK;
}


E1701_API long ScSCIGetGain(double *GainX, double *GainY)
{
   *GainX=data[currHead].gainX; *GainY=data[currHead].gainY;
   return SC_OK;
}


E1701_API long ScSCISetRotAngle(double Angle) 
{
   data[currHead].rot=Angle;
   E1701_set_xy_correction(currHead,data[currHead].gainX,data[currHead].gainY,data[currHead].rot,0,0); // add offsetX/Y during wrapped jump and mark calls
   return SC_OK;
}


E1701_API long ScSCIGetRotAngle(double *Angle)
{
   *Angle=data[currHead].rot;
   return SC_OK;
}


E1701_API long ScSCISetOffset(double OffsetX, double OffsetY)
{
   data[currHead].offsetX=OffsetX; data[currHead].offsetY=OffsetY;
   return SC_OK;
}


E1701_API long ScSCIGetOffset(double *OffsetX, double *OffsetY)
{
   *OffsetX=data[currHead].offsetX; *OffsetY=data[currHead].offsetY;
   return SC_OK;
}


E1701_API long ScSCISetMoveLaserState(long Value)
{
   data[currHead].laserState=(unsigned char)Value;
   return SC_OK;
}

E1701_API long ScSCIGetMoveLaserState(long *Value)
{
   *Value=data[currHead].laserState;
   return SC_OK;
}


E1701_API long ScSCIMoveAbs3D(double X,double Y,double Z)
{
   int x,y,z;
   int ret;

   x=(int)OAPC_ROUND((X+data[currHead].offsetX+data[currHead].fieldOffsetX)*data[currHead].factor*1024.0,0);
   y=(int)OAPC_ROUND((Y-data[currHead].offsetY+data[currHead].fieldOffsetY)*data[currHead].factor*1024.0,0);
   z=(int)OAPC_ROUND(Z*data[currHead].factor*1024.0,0);
   if (data[currHead].laserState==1) ret=E1701_mark_abs(currHead,x,y,z);
   else ret=E1701_jump_abs(currHead,x,y,z);

   if (ret==E1701_OK) return SC_OK;
   return SC_ERROR;
}


E1701_API long ScSCIMoveAbs(double X,double Y)
{
   return ScSCIMoveAbs3D(X,Y,0.0);
}


E1701_API long ScSCIMarkLine3D(double x0,double y0,double z0,double x1,double y1,double z1)
{
   int x,y,z;
   int ret;

   x=(int)OAPC_ROUND((x0+data[currHead].offsetX+data[currHead].fieldOffsetX)*data[currHead].factor*1024.0,0);
   y=(int)OAPC_ROUND((y0-data[currHead].offsetY+data[currHead].fieldOffsetY)*data[currHead].factor*1024.0,0);
   z=(int)OAPC_ROUND(z0*data[currHead].factor,0);
   ret=E1701_jump_abs(currHead,x,y,z);
   if (ret!=E1701_OK) return SC_ERROR;
   x=(int)OAPC_ROUND((x1+data[currHead].offsetX+data[currHead].fieldOffsetX)*data[currHead].factor*1024.0,0);
   y=(int)OAPC_ROUND((y1-data[currHead].offsetY+data[currHead].fieldOffsetY)*data[currHead].factor*1024.0,0);
   z=(int)OAPC_ROUND(z1*data[currHead].factor,0);
   ret=E1701_mark_abs(currHead,x,y,z);
   if (ret!=E1701_OK) return SC_ERROR;
   
   return SC_OK;
}


E1701_API long ScSCIMarkLine(double x0, double y0, double x1, double y1)
{
   return ScSCIMarkLine3D(x0,y0,0.0,x1,y1,0.0);
}


E1701_API long ScSCISetDeviceOperationMode(long Value)
{
   data[currHead].standardDeviceOperationMode=Value;
   
   if ((data[currHead].standardDeviceOperationMode & scComStandardDeviceOperationModeIdCO2)==scComStandardDeviceOperationModeIdCO2)
    data[currHead].laserMode=E1701_LASERMODE_CO2;
   else if ((data[currHead].standardDeviceOperationMode & scComStandardDeviceOperationModeIdYAG)==scComStandardDeviceOperationModeIdYAG)
    data[currHead].laserMode=E1701_LASERMODE_YAG1;
   else if ((data[currHead].standardDeviceOperationMode & scComStandardDeviceOperationModeYAG2)==scComStandardDeviceOperationModeYAG2)
    data[currHead].laserMode=E1701_LASERMODE_YAG2;
   else if ((data[currHead].standardDeviceOperationMode & scComStandardDeviceOperationModeIdIPG)==scComStandardDeviceOperationModeIdIPG)
    data[currHead].laserMode=E1701_LASERMODE_CRF;
   else data[currHead].laserMode=E1701_LASERMODE_YAG1;

   E1701_set_laser_mode(currHead,data[currHead].laserMode);

   return SC_OK;
}


E1701_API long ScSCIGetDeviceOperationMode(long *Value)
{
   *Value=data[currHead].standardDeviceOperationMode;
   return SC_OK;
}


E1701_API long ScSCISetDeviceTimer(long TimerId,double Value) 
{
   if (TimerId==scComStandardDeviceIDHalfPeriodStandBy)
   {
      data[currHead].halfPeriodStandBy=Value;
      E1701_set_standby(currHead,1.0/(data[currHead].halfPeriodStandBy*2.0),data[currHead].standByPulse*1000000.0);
   }
   else if (TimerId==scComStandardDeviceIDStandByPulse)
   {
      data[currHead].standByPulse=Value;
      E1701_set_standby(currHead,1.0/(data[currHead].halfPeriodStandBy*2.0),data[currHead].standByPulse*1000000.0);
   }
   else if (TimerId==scComStandardDeviceStyleIDFirstPulseLength)
   {
      double yag3QTime=0;

      data[currHead].firstPulseLength=Value;
      if (data[currHead].laserMode==E1701_LASERMODE_YAG2) yag3QTime=data[currHead].firstPulseLength;
      E1701_set_fpk(currHead,data[currHead].firstPulseLength,yag3QTime);
   }
   else if (TimerId==scComStandardDeviceStyleIDWobblePeriod)
   {
      data[currHead].wobblePeriod=Value;
      E1701_set_wobble(currHead,(unsigned short)data[currHead].miscWobbleAmp,(unsigned short)data[currHead].miscWobbleAmp,1.0/data[currHead].wobblePeriod);
   }
   else if (TimerId==scComStandardDeviceStyleIDHalfPeriodCO2)
   {
      data[currHead].halfPeriodCO2=Value;
      E1701_set_laser_timing(currHead,1.0/(data[currHead].halfPeriodCO2*2.0),data[currHead].laser1Length*1000000.0);
   }
   else if (TimerId==scComStandardDeviceStyleIDHalfPeriod)
   {
      data[currHead].halfPeriod=Value;
      E1701_set_laser_timing(currHead,1.0/(data[currHead].halfPeriod*2.0),data[currHead].laser1Length*1000000.0);
   }
   else if (TimerId==scComStandardDeviceStyleIDLaser1Length)
   {
      data[currHead].halfPeriodCO2=Value;
      if ((data[currHead].standardDeviceOperationMode & scComStandardDeviceOperationModeIdCO2)==scComStandardDeviceOperationModeIdCO2)
       E1701_set_laser_timing(currHead,1.0/(data[currHead].halfPeriodCO2*2.0),data[currHead].laser1Length*1000000.0);
      else
       E1701_set_laser_timing(currHead,1.0/(data[currHead].halfPeriod*2.0),data[currHead].laser1Length*1000000.0);
   }
   else if (TimerId==scComStandardDeviceStyleIDQSwitchPeriod)
   {
      data[currHead].qSwitchPeriod=Value;
      E1701_set_laser_timing(currHead,1.0/(data[currHead].qSwitchPeriod*2.0),data[currHead].qSwitchLength*1000000.0);
   }
   else if (TimerId==scComStandardDeviceStyleIDQSwitchLength)
   {
      data[currHead].qSwitchLength=Value;
      E1701_set_laser_timing(currHead,1.0/(data[currHead].qSwitchPeriod*2.0),data[currHead].qSwitchLength*1000000.0);
   }
   else
   {
      assert(0);
      return SC_ERROR;
   }
   return SC_OK;
}


E1701_API long ScSCIGetDeviceTimer(long TimerId,double *Value)
{
   if (TimerId==scComStandardDeviceIDHalfPeriodStandBy)          *Value=data[currHead].halfPeriodStandBy;
   else if (TimerId==scComStandardDeviceIDStandByPulse)          *Value=data[currHead].standByPulse;
   else if (TimerId==scComStandardDeviceStyleIDFirstPulseLength) *Value=data[currHead].firstPulseLength;
   else if (TimerId==scComStandardDeviceStyleIDWobblePeriod)     *Value=data[currHead].wobblePeriod;
   else if (TimerId==scComStandardDeviceStyleIDHalfPeriodCO2)    *Value=data[currHead].halfPeriodCO2;
   else if (TimerId==scComStandardDeviceStyleIDHalfPeriod)       *Value=data[currHead].halfPeriod;
   else if (TimerId==scComStandardDeviceStyleIDLaser1Length)     *Value=data[currHead].laser1Length;
   else if (TimerId==scComStandardDeviceStyleIDQSwitchPeriod)    *Value=data[currHead].qSwitchPeriod;
   else if (TimerId==scComStandardDeviceStyleIDQSwitchLength)    *Value=data[currHead].qSwitchLength;
   else
   {
      assert(0);
      return SC_ERROR;
   }
   return SC_OK;
}


E1701_API long ScSCISetDeviceMiscValue(long MiscId,long Value)
{
   if (MiscId==scComStandardDeviceStyleIDMiscWobbleAmp)
   {
      data[currHead].miscWobbleAmp=Value;
      E1701_set_wobble(currHead,(unsigned short)data[currHead].miscWobbleAmp,(unsigned short)data[currHead].miscWobbleAmp,1.0/data[currHead].wobblePeriod);
   }
   else if (MiscId==scComStandardDeviceMiscEncoderKx)
   {
      data[currHead].miscEncoderKx=Value;
      E1701_digi_set_motf(currHead,data[currHead].miscEncoderKx,data[currHead].miscEncoderKy);
   }
   else if (MiscId==scComStandardDeviceMiscEncoderKy)
   {
      data[currHead].miscEncoderKy=Value;
      E1701_digi_set_motf(currHead,data[currHead].miscEncoderKx,data[currHead].miscEncoderKy);
   }
   else
   {
      assert(0);
      return SC_ERROR;
   }
   return SC_OK;
}

E1701_API long ScSCIGetDeviceMiscValue(long MiscId,long *Value)
{
   if (MiscId==scComStandardDeviceStyleIDMiscWobbleAmp) *Value=data[currHead].miscWobbleAmp;
   else if (MiscId==scComStandardDeviceMiscEncoderKx)   *Value=data[currHead].miscEncoderKx;
   else if (MiscId==scComStandardDeviceMiscEncoderKy)   *Value=data[currHead].miscEncoderKy;
   else
   {
      assert(0);
      return SC_ERROR;
   }
   return SC_OK;
}


E1701_API long ScSCISetExecute(long Value)
{
   if (Value==0) E1701_stop_execution(currHead);
   data[currHead].isExecuting=Value;
   return SC_OK;
}


E1701_API long ScSCIGetExecute(long *Value)
{
   *Value=data[currHead].isExecuting;
   return SC_OK;
}


E1701_API long ScSCIIsExposureEnd(long *Value)
{  
   if ((E1701_get_card_state(currHead) & E1701_CSTATE_MARKING)==E1701_CSTATE_MARKING) *Value=1;
   else *Value=0;
   return SC_OK;
}


E1701_API long ScSCISetDeviceDelay(long DelayId, double Value)
{
   if (DelayId==scComStandardDeviceStyleIDMarkDelay)
   {
      data[currHead].markDelay=Value;
      E1701_set_scanner_delays(currHead,data[currHead].jumpDelay,data[currHead].markDelay,data[currHead].polyDelay);
   }
   else if (DelayId==scComStandardDeviceStyleIDJumpDelay)
   {
      data[currHead].jumpDelay=Value;
      E1701_set_scanner_delays(currHead,data[currHead].jumpDelay,data[currHead].markDelay,data[currHead].polyDelay);
   }
   else if (DelayId==scComStandardDeviceStyleIDPolyDelay)
   {
      data[currHead].polyDelay=Value;
      E1701_set_scanner_delays(currHead,data[currHead].jumpDelay,data[currHead].markDelay,data[currHead].polyDelay);
   }
   else if (DelayId==scComStandardDeviceStyleIDLaserOnDelay)
   {
      data[currHead].laserOnDelay=Value;
      E1701_set_laser_delays(currHead,data[currHead].laserOnDelay,data[currHead].laserOffDelay);
   }
   else if (DelayId==scComStandardDeviceStyleIDLaserOffDelay)
   {
      data[currHead].laserOffDelay=Value;
      E1701_set_laser_delays(currHead,data[currHead].laserOnDelay,data[currHead].laserOffDelay);
   }
   else if (DelayId==scComStandardDeviceStreamIDLongDelay)
   {
      data[currHead].longDelay=Value;
      E1701_delay(currHead,data[currHead].longDelay);
   }
   else
   {
      assert(0);
      return SC_ERROR;
   }
   return SC_OK;
}


E1701_API long ScSCIGetDeviceDelayD(long DelayId, double *Value)
{
   if (DelayId==scComStandardDeviceStyleIDMarkDelay)          *Value=data[currHead].markDelay;
   else if (DelayId==scComStandardDeviceStyleIDJumpDelay)     *Value=data[currHead].jumpDelay;
   else if (DelayId==scComStandardDeviceStyleIDPolyDelay)     *Value=data[currHead].polyDelay;
   else if (DelayId==scComStandardDeviceStyleIDLaserOnDelay)  *Value=data[currHead].laserOnDelay;
   else if (DelayId==scComStandardDeviceStyleIDLaserOffDelay) *Value=data[currHead].laserOffDelay;
   else if (DelayId==scComStandardDeviceStreamIDLongDelay)    *Value=data[currHead].longDelay;
   else
   {
      assert(0);
      return SC_ERROR;
   }
   return SC_OK;
}


E1701_API long ScSCIGetDeviceDelay(long DelayId, long *Value)
{
   double dValue;
   long   ret;

   ret=ScSCIGetDeviceDelayD(DelayId,&dValue);
   *Value=(long)OAPC_ROUND(dValue,0);
   return ret;
}


E1701_API long ScSCISetDeviceDelayD(long DelayId, double Value)
{
   return ScSCISetDeviceDelay(DelayId,Value);
}


E1701_API long ScSCISetDeviceSpeed(long SpeedId,double Value)
{
   if (SpeedId==scComStandardDeviceStyleIDJumpSpeed)
   {
      data[currHead].jumpSpeed=Value;
      E1701_set_speeds(currHead,data[currHead].jumpSpeed*data[currHead].factor,data[currHead].markSpeed*data[currHead].factor);
   }
   else if (SpeedId==scComStandardDeviceStyleIDMarkSpeed)
   {
      data[currHead].markSpeed=Value;
      E1701_set_speeds(currHead,data[currHead].jumpSpeed*data[currHead].factor,data[currHead].markSpeed*data[currHead].factor);
   }
   else
   {
       assert(0);
       return SC_ERROR;
   }
   return SC_OK;
}


E1701_API long ScSCIGetDeviceSpeed(long SpeedId,double *Value)
{
   if (SpeedId==scComStandardDeviceStyleIDJumpSpeed)      *Value=data[currHead].jumpSpeed;
   else if (SpeedId==scComStandardDeviceStyleIDMarkSpeed) *Value=data[currHead].jumpSpeed;
   else
   {
      assert(0);
      return SC_ERROR;
   }
   return SC_OK;
}


E1701_API long ScSCISetDevicePortValue(long PortId,long Value)
{
   if ((PortId==scComStandardDeviceControlIDPort2) || (PortId==scComStandardDeviceControlIDPortLaser))
   {
      if ((data[currHead].standardDeviceOperationMode & scComStandardDeviceOperationModeIdIPG)==scComStandardDeviceOperationModeIdIPG)
       E1701_lp8_write_latch(currHead,1,50,(unsigned char)Value,2,2);
      else E1701_lp8_write(currHead,(unsigned char)Value);
   }
   else if (PortId==scComStandardDeviceControlIDPortOptoOut) E1701_digi_write(currHead,(unsigned char)Value);
   else if (PortId==scComStandardDeviceStreamIDPortDA1)      E1701_lp8_a0(currHead,(unsigned char)Value);
   else
   {
       assert(0);
       return SC_ERROR;
   }
   return SC_OK;
}


E1701_API long ScSCISetExternalTrigger(long Value)
{
   if (Value!=0) E1701_set_trigger_point(currHead);
   return SC_OK;
}


E1701_API long ScSCIFlush(void)
{
   E1701_execute(currHead);
   return SC_OK;
}


E1701_API long ScSCIGetDevicePortValue(long PortId,long *Value)
{
   if (PortId==scComStandardDeviceIDPortOptoIn) *Value=E1701_digi_read(currHead);
   else
   {
      assert(0);
      return SC_ERROR;
   }
   return SC_OK;
}


E1701_API void ScSCIExit(void)
{
   E1701_close(currHead);
}


#endif // ENV_WINDOWS -- SCI interface is Windows-only
