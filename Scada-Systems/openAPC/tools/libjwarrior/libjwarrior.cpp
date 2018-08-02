/**
 * libjwarrior - library for easy data access of JoyWarrior accelerometers
 * Copyright (C) 2010-2011 OpenAPC Inc. openapc(at)gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This software is not designed or intended for use in on-line control of
 * aircraft, air traffic, aircraft navigation or aircraft communications; or in
 * the design, construction, operation or maintenance of any nuclear
 * facility. Licensee represents and warrants that it will not use or
 * redistribute the Software for such purposes.
 */


#include "libjwarrior.h"
#include "oapc_libio.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>

static void *instData;
static double accXSum,accYSum,accZSum,retSpdX,retSpdY,retSpdZ;
static int    sumCntX,sumCntY,sumCntZ;
static pthread_mutex_t ioMutex;



/**
 * Internal function: here the data from the thread of libio_jwarrior.cpp are received
 * and collected; please refer to oapc_libio.h / the OpenAPC SDK manual for more information
 * @param[in] output the output for which the function is called and for which new data can
 *            be fetched now
 * @param[in] callbackID the identifier of the callback, can be ignored here
 */
static void plugin_io_callback(unsigned long output,unsigned long callbackID)
{
   double d;

printf("Getting data for IO %lX\n",output);   
   callbackID=callbackID; // to avoid "unused variable" warning
   pthread_mutex_lock(&ioMutex);
   if (output==OAPC_NUM_IO0)
   {
      oapc_get_num_value(instData,0,&d);
      accXSum+=d;
      sumCntX++;
   }
   else if (output==OAPC_NUM_IO1)
   {
      oapc_get_num_value(instData,1,&d);
      accYSum+=d;
      sumCntY++;
   }
   else if (output==OAPC_NUM_IO2)
   {
      oapc_get_num_value(instData,2,&d);
      accZSum+=d;
      sumCntZ++;
   }
   else if (output==OAPC_NUM_IO3) oapc_get_num_value(instData,3,&retSpdX);
   else if (output==OAPC_NUM_IO4) oapc_get_num_value(instData,4,&retSpdY);
   else if (output==OAPC_NUM_IO5) oapc_get_num_value(instData,5,&retSpdZ);
   pthread_mutex_unlock(&ioMutex);
}



/**
 * Opens the JoyWarrior device and initialize everything
 * @param[in] device the device to be used for the accelerometer, e.g. /dev/input/js0
 * @param[in] maxG a constant of type LJW_MAX_G_x that specifies for which maximum G the
 *            device was programmed; this value has to be set in order to have a correct
 *            acceleration and speed calculation
 * @return 0 in case everything could be initialized successful or a negative number in
 *            case of an error; this negative error code is equal to the (positive) error
 *            constants of errno.h and further specifies the problem that occured within
 *            this function
 */
int ljw_open(const char *device,int maxG)
{
   int ret;
   char val[100];
   
   instData=oapc_create_instance2(0);
   if (instData)
   {
      oapc_set_config_data(instData,"port",(char*)device);
      sprintf(val,"%d",maxG);
      oapc_set_config_data(instData,"maxg",val);
      oapc_set_io_callback(instData,plugin_io_callback,42);
      ret=oapc_init(instData);
      if (ret==OAPC_OK)
      {
         accXSum=0.0;   sumCntX=0;
         accYSum=0.0;   sumCntY=0;
         accZSum=0.0;   sumCntZ=0;
         ret=pthread_mutex_init(&ioMutex,NULL);
         if (ret!=0)
         {
            oapc_exit(instData);
            oapc_delete_instance(instData);
            printf("Error: could not create mutex (%d)!\n",ret);
            return -ret;
         }
         printf("Initialization successful\n");
         return 0;
      }
      else
      {
         printf("Error: could not initialize (%d)!\n",ret);
         oapc_delete_instance(instData);
         return -ENXIO;
      }
   }
   else return -ENOMEM;
   return 1; // should never happen
}



/**
 * This function can be used to retrieve new acceleration and speed data from the device. It should
 * be polled periodically in order to get the latest measured values. Here the return value decides
 * if new data are available and if the contents of the handed over pointers have been changed or
 * not; only in case a 0 is returned the data can be used
 * @param[out] accX the average acceleration in X-direction in unit m*s^-2 since the last call to
 *             ljw_get_data()
 * @param[out] accY the average acceleration in Y-direction in unit m*s^-2 since the last call to
 *             ljw_get_data()
 * @param[out] accZ the average acceleration in Z-direction in unit m*s^-2 since the last call to
 *             ljw_get_data()
 * @param[out] spdX the current speed in X-direction in unit m*s⁻1 calculated out of the measured
 *             acceleration data
 * @param[out] spdY the current speed in Y-direction in unit m*s⁻1 calculated out of the measured
 *             acceleration data
 * @param[out] spdZ the current speed in Z-direction in unit m*s⁻1 calculated out of the measured
 *             acceleration data
 * @return -EINVAL in case no new data are available since the last call to ljw_get_data(), in this
 *             case the contents of the pointers to the acceleration and speed variables are left
 *             unchanged or 0 in case new data are available, in this case the contents of all
 *             acceleration and speed variables are updated
 */
int ljw_get_data(double *accX,double *accY,double *accZ,double *spdX,double *spdY,double *spdZ)
{
   pthread_mutex_lock(&ioMutex);
printf("ljw_get_data: %d %d %d %f %f %f %f %f %f\n",sumCntX,sumCntY,sumCntZ,accXSum,accYSum,accZSum,retSpdX,retSpdY,retSpdZ);
   if ((sumCntX==0) && (sumCntY==0) && (sumCntZ==0))
   {
      pthread_mutex_unlock(&ioMutex);
      return -EAGAIN;
   }
   if (sumCntX>0)
   {
      *accX=accXSum/sumCntX; 
      accXSum=0.0; 
      sumCntX=0;
   }
   else *accX=0;
   if (sumCntY>0)
   {
      *accY=accYSum/sumCntY;
      accYSum=0.0;
      sumCntY=0;
   }
   else *accY=0;
   if (sumCntZ>0)
   {
      *accZ=accZSum/sumCntZ;
      accZSum=0.0;
      sumCntZ=0;
   }
   else *accZ=0;
   *spdX=retSpdX;
   *spdY=retSpdY;
   *spdZ=retSpdZ;
   pthread_mutex_unlock(&ioMutex);
   return 0;
}



/**
 * The speed calculated out of the acceleration values may differ from the real speed. It depends on many
 * factors how big thar difference becomes over time (speed and strength of acceleration change, internal
 * calculation accuracy, variation of the hardware,...). Using this function the speed can be corrected
 * (e.g. by using a different data source for the speed).
 * @param[in] speed a constant of type LJW_SPEED_x that decides if the new speed value has to be set for
 *            X, Y or Z direction
 * @param[in] value the new speed value in unit m*s^-1 that corrects the current speed calculation
 * @return 0 in case the function could be finished successfully or a negative number in case of an error;
 *            this negative error code is equal to one of the (positive) error constants of errno.h and
 *            specifies the problem that occured within this function
 */
int ljw_correct_speed(int speed,double value)
{
   if (speed==LJW_SPEED_X) oapc_set_num_value(instData,3,value);
   else if (speed==LJW_SPEED_Y) oapc_set_num_value(instData,4,value);
   else if (speed==LJW_SPEED_Z) oapc_set_num_value(instData,5,value);
   else return -EINVAL;
   return 0;
}



/**
 * Closes the connection to the JoyWarrior device and releases all resources; after this function was used
 * it is not allowed to call any other function of this library except ljw_open()
 * @return 0 in case the function could be finished successfully or a negative number in case of an error;
 *            this negative error code is equal to one of the (positive) error constants of errno.h and
 *            specifies the problem that occured within this function
 */
int ljw_close()
{
   oapc_exit(instData);
   oapc_delete_instance(instData);
   pthread_mutex_destroy(&ioMutex);
   return 0;
}





