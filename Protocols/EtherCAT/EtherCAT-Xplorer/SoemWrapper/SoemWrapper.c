/*
 * SOEM Lib
 * slaveinfo & simpletest & eepromtool
 * http://openethercatsociety.github.io/
 *
 * (c)Arthur Ketels 2010 - 2011
 *
 * For JNI string & array manipulation thank's to
 * http://electrofriends.com/category/articles/jni/
 */

/**************************************************************************
*                           MIT License
* 
* Copyright (C) 2016 Frederic Chaxel <fchaxel@free.fr>
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*********************************************************************/
#include <stdio.h>
#include <string.h>

#include "ethercattype.h"
#include "nicdrv.h"
#include "ethercatbase.h"
#include "ethercatmain.h"
#include "ethercatconfig.h"
#include "ethercatcoe.h"
#include "ethercatdc.h"
#include "ethercatprint.h"

#include "jni.h"

char* IOmap=NULL;
UINT tmrThread=0;
int DelayUp;

////////////////////////////////////////////////////////////////////////////////////////////////
__declspec( dllexport ) int GetDevicesNumber()
{
	return ec_slavecount;
}
JNIEXPORT jint JNICALL Java_soem_SoemInterrop_GetDevicesNumber (JNIEnv * jenv, jclass jclasse)
{
	return GetDevicesNumber();
}
////////////////////////////////////////////////////////////////////////////////////////////////
enum DeviceInfoParam {
        Name = 0,
        OutputSize= 1,
        InputSize = 2,
        State = 4,
        Delay = 5,
        Config_Address = 6,
        ManufacturerId = 7,
        TypeId = 8,
        Rev = 9,
		MailboxProtocol = 10,
}  ;
////////////////////////////////////////////////////////////////////////////////////////////////
// Get, in String format, a device attribut
// Very slow, but simple to write !, and no need of struct
////////////////////////////////////////////////////////////////////////////////////////////////
__declspec( dllexport ) int GetDeviceInfo(unsigned int deviceNum, int ParamNumber, LPSTR buf)
{
	buf[0]=0;
	if (deviceNum>(unsigned int)ec_slavecount) return -1;

	switch ((enum DeviceInfoParam)ParamNumber)
	{
		case Name:
			sprintf (buf,"%s",ec_slave[deviceNum].name);
			break;
		case OutputSize:
			sprintf (buf,"%u",ec_slave[deviceNum].Obits);
			break;
		case InputSize:
			sprintf (buf,"%u",ec_slave[deviceNum].Ibits);
			break;
		case State:
			sprintf (buf,"%u",ec_slave[deviceNum].state);
			break;
		case Delay:
			sprintf (buf,"%d",ec_slave[deviceNum].pdelay);
			break;
		case Config_Address:
			sprintf(buf,"%4.4x", ec_slave[deviceNum].configadr);
			break;
		case ManufacturerId:
			sprintf (buf,"%u",ec_slave[deviceNum].eep_man);
			break;
		case TypeId:
			sprintf (buf,"%u",ec_slave[deviceNum].eep_id);
			break;
		case Rev:
			sprintf (buf,"%u",ec_slave[deviceNum].eep_rev);
			break;
		case MailboxProtocol:
			sprintf (buf,"%u",ec_slave[deviceNum].mbx_proto);
			break;	
		// To get some parameters (during Debug)
		case -2:
			sprintf (buf,"debug");
			break;
	    default :
			ParamNumber=-1;
	}
	return ParamNumber;
}
JNIEXPORT jstring Java_soem_SoemInterrop_GetDeviceInfo(JNIEnv *jenv, jclass jclasse, jint deviceNum, jint ParamNumber)
{
	jstring ret;
	int isOk;

	LPSTR buf=(LPSTR)malloc(1024);
	isOk=GetDeviceInfo(deviceNum, ParamNumber, buf);
	if (isOk!=-1)
		ret= (*jenv)->NewStringUTF(jenv, buf);
	else
		ret= (*jenv)->NewStringUTF(jenv, "");

	free(buf);
	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////
__declspec( dllexport ) int EEprom_Read(unsigned int  deviceNum, int start, int length, char* buf)
{
	int i, ainc = 4;
	uint16 estat, aiadr;
	uint32 b4;
	uint64 b8;
	uint8 eepctl;
   
	// direct copy of eepromtool.c, (c)Arthur Ketels 2010

	aiadr = 1 - deviceNum;
	eepctl = 2;
	if (ec_APWR(aiadr, ECT_REG_EEPCFG, sizeof(eepctl), &eepctl , EC_TIMEOUTRET)==0) return 0; /* force Eeprom from PDI */
	eepctl = 0;
	if (ec_APWR(aiadr, ECT_REG_EEPCFG, sizeof(eepctl), &eepctl , EC_TIMEOUTRET)==0) return 0; /* set Eeprom to master */

	estat = 0x0000;
	ec_APRD(aiadr, ECT_REG_EEPSTAT, sizeof(estat), &estat, EC_TIMEOUTRET); /* read eeprom status */
	estat = etohs(estat);
	if (estat & EC_ESTAT_R64)
	{
		ainc = 8;
		for (i = start ; i < (start + length) ; i+=ainc)
		{
			b8 = ec_readeepromAP(aiadr, i >> 1 , EC_TIMEOUTEEP);
			buf[i] = b8&0xFF;
			buf[i+1] = (b8 >> 8)&0xFF;
			buf[i+2] = (b8 >> 16)&0xFF;
			buf[i+3] = (b8 >> 24)&0xFF;
			buf[i+4] = (b8 >> 32)&0xFF;
			buf[i+5] = (b8 >> 40)&0xFF;
			buf[i+6] = (b8 >> 48)&0xFF;
			buf[i+7] = (b8 >> 56)&0xFF;
		}
	}
	else
	{
		for (i = start ; i < (start + length) ; i+=ainc)
		{
			b4 = (uint32)ec_readeepromAP(aiadr, i >> 1 , EC_TIMEOUTEEP);
			buf[i] = b4;
			buf[i+1] = b4 >> 8;
			buf[i+2] = b4 >> 16;
			buf[i+3] = b4 >> 24;
		}
	}
      
	return 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////
__declspec( dllexport ) int EEprom_Write(unsigned int deviceNum, int start, int length, char* buf)
{
	int i;
	uint16 aiadr;
	uint8 eepctl;
	int ret;

	// direct copy of eepromtool.c, (c)Arthur Ketels 2010
   
	aiadr = 1 - deviceNum;
	eepctl = 2;
	if (ec_APWR(aiadr, ECT_REG_EEPCFG, sizeof(eepctl), &eepctl , EC_TIMEOUTRET)==0) return 0; /* force Eeprom from PDI */
	eepctl = 0;
	if (ec_APWR(aiadr, ECT_REG_EEPCFG, sizeof(eepctl), &eepctl , EC_TIMEOUTRET)==0) return 0; /* set Eeprom to master */

	for (i = start ; i < (start + length) ; i+=2)
	{
		uint16 data=buf[start]+(((uint16)buf[start+1])<<8);
		ret = ec_writeeepromAP(aiadr, i >> 1 , data, EC_TIMEOUTEEP);
	}      
	return 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////
__declspec( dllexport ) int GetInput(unsigned int deviceNum, char *buf)
{
	memcpy(buf,ec_slave[deviceNum].inputs,ec_slave[deviceNum].Ibytes);
	return ec_slave[deviceNum].Ibytes;
}
JNIEXPORT jint JNICALL Java_soem_SoemInterrop_GetInput (JNIEnv * jenv, jclass jclasse, jint deviceNum, jbyteArray buf)
{
	int ret;
	jbyte *body = (*jenv)->GetByteArrayElements(jenv,buf,0);
	ret = GetInput(deviceNum, (char*)body);
	(*jenv)->ReleaseByteArrayElements(jenv, buf, body, 0);
	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////
__declspec( dllexport ) int SetOutput(unsigned int deviceNum, char *buf)
{
	memcpy(ec_slave[deviceNum].outputs, buf, ec_slave[deviceNum].Obytes);
	return ec_slave[deviceNum].Obytes; 
}
JNIEXPORT jint JNICALL Java_soem_SoemInterrop_SetOutput(JNIEnv * jenv, jclass jclasse, jint deviceNum, jbyteArray buf)
{
	int ret;
	jbyte *body = (*jenv)->GetByteArrayElements(jenv,buf,0);
	ret = SetOutput(deviceNum, (char*)body);
	(*jenv)->ReleaseByteArrayElements(jenv, buf, body, 0);

	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////
__declspec( dllexport ) int ReadPDO(unsigned int deviceNum, int Index, 
									int SubIndex,  int *size, char *buf)
{
	int __ret;

	// FALSE -> TRUE for all subIndex readout
	boolean Mode=FALSE;
	if (SubIndex==-1) 
	{ 
		Mode=TRUE;
		SubIndex=0;
	}

	__ret = ec_SDOread(deviceNum, Index, SubIndex, Mode, size, buf, EC_TIMEOUTRXM); 

	return __ret;
}
JNIEXPORT jint JNICALL Java_soem_SoemInterrop_ReadPDO (JNIEnv * jenv, jclass jclasse, jint deviceNum, jint Index, jint SubIndex, jint size, jbyteArray buf)
{
	int size2=size;	
	int ret;

	jbyte *body = (*jenv)->GetByteArrayElements(jenv,buf,0);
	ret= ReadPDO(deviceNum, Index, SubIndex,&size2,(char*)body);
	(*jenv)->ReleaseByteArrayElements(jenv, buf, body, 0);

	if (ret==-1) return -1; else return size2;

}
////////////////////////////////////////////////////////////////////////////////////////////////
__declspec( dllexport ) int WritePDO(unsigned int deviceNum, int Index, 
									int SubIndex, int size, char *buf)
{
	int __ret;
	// FALSE -> TRUE for all subIndex writeout
	boolean Mode=FALSE;
	if (SubIndex==-1) 
	{ 
		Mode=TRUE;
		SubIndex=0;
	}

	 __ret = ec_SDOwrite(deviceNum, Index, SubIndex, Mode, size, buf, EC_TIMEOUTRXM); 

	return __ret;
}
JNIEXPORT jint JNICALL Java_soem_SoemInterrop_WritePDO (JNIEnv * jenv, jclass jclasse, jint deviceNum, jint Index, jint SubIndex, jint size, jbyteArray buf)
{
	int ret;

	jbyte *body = (*jenv)->GetByteArrayElements(jenv,buf,0);
	ret= WritePDO(deviceNum, Index, SubIndex,size,(char*)buf);
	(*jenv)->ReleaseByteArrayElements(jenv, buf, body, 0);

	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////
__declspec( dllexport ) void RefreshSlavesState()
{
	ec_readstate();
}
JNIEXPORT void JNICALL jni_RefreshSlavesState (JNIEnv * jenv, jclass jclasse)
{
	 RefreshSlavesState();
}
////////////////////////////////////////////////////////////////////////////////////////////////
#define CYCLE_PERIOD 10
void CALLBACK RTthread(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1,  DWORD_PTR dw2)
{  
    ec_send_processdata();
    ec_receive_processdata(EC_TIMEOUTRET);
}
////////////////////////////////////////////////////////////////////////////////////////////////
__declspec( dllexport ) void Run(unsigned short State)
{    

	/* send one valid process data to make outputs in slaves happy*/
	ec_send_processdata();
	ec_receive_processdata(EC_TIMEOUTRET);

	tmrThread = timeSetEvent(CYCLE_PERIOD, 0, RTthread, 0, TIME_PERIODIC|TIME_KILL_SYNCHRONOUS);

	if (State!=EC_STATE_OPERATIONAL) return;

	ec_slave[0].state = EC_STATE_OPERATIONAL;
	ec_writestate(0);

}
JNIEXPORT void JNICALL Java_soem_SoemInterrop_Run (JNIEnv * jenv, jclass jclasse, jshort State)
{
	 Run(State);
}
////////////////////////////////////////////////////////////////////////////////////////////////
__declspec( dllexport ) void WriteState(unsigned int deviceNum, unsigned short State)
{
	ec_slave[deviceNum].state = State;
	ec_writestate(deviceNum);
}
JNIEXPORT void JNICALL Java_soem_SoemInterrop_WriteState (JNIEnv * jenv, jclass jclasse, jint deviceNum, jshort State)
{
	 WriteState(deviceNum, State);
}
////////////////////////////////////////////////////////////////////////////////////////////////
__declspec( dllexport ) void Reconfigure(unsigned int deviceNum)
{
	ec_reconfig_slave(deviceNum,EC_TIMEOUTRET);
}
JNIEXPORT void JNICALL Java_soem_SoemInterrop_Reconfigure (JNIEnv * jenv, jclass jclasse, jint deviceNum)
{
	 Reconfigure(deviceNum);
}
////////////////////////////////////////////////////////////////////////////////////////////////
__declspec( dllexport ) int StartActivity(LPCTSTR ifaceName, int DelayUpMs)
{
	/* initialise SOEM, bind socket to ifname */
	if (ec_init((char*)ifaceName))
    { 
		/* find and auto-config slaves */
		if (ec_config_init(FALSE)>0) // return number of slaves
		{
			size_t size;
			int i;

			// first discovers IO size and map input output on a dummy 
			// memory zone starting at zero
			ec_config_map(NULL);

			// calculate the required IO size
			size=ec_slave[0].Ibytes+ec_slave[0].Obytes+2; // more or equal to needed

			// Create the final IOMap
			IOmap=(char*)malloc(size);
			memset((void*)IOmap,0,size);

			// shift all pointers into the IOMap instead of dummy zone (used and unused)
			for (i=0;i<EC_MAXSLAVE;i++)
			{
				ec_slave[i].outputs=(uint8 *)((int)IOmap+(int)ec_slave[i].outputs);	
				ec_slave[i].inputs=(uint8 *)((int)IOmap+(int)ec_slave[i].inputs);
			}
			for (i=0;i<EC_MAXGROUP;i++)
			{
				ec_group[i].inputs=(uint8 *)((int)IOmap+(int)ec_group[i].inputs);
				ec_group[i].outputs=(uint8 *)((int)IOmap+(int)ec_group[i].outputs);
			}

			// distributed clock config
			ec_configdc(); 

			return ec_slavecount; 
			
		}
		else
		{   // No slave found, stop SOEM, close socket
			ec_close();
			return 0;
		}
	}
	return -1;
}
JNIEXPORT jint JNICALL Java_soem_SoemInterrop_StartActivity (JNIEnv * jenv, jclass jclasse, jstring ifaceName, jint DelayUpMs)
{
	const char *_ifaceName= (*jenv)->GetStringUTFChars(jenv,ifaceName,0);
	int ret= StartActivity(_ifaceName, DelayUpMs);
	(*jenv)->ReleaseStringUTFChars(jenv, ifaceName, _ifaceName);
	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////
__declspec( dllexport ) void StopActivity()
{	
	if (tmrThread!=0)
         /* stop RT thread */
         timeKillEvent(tmrThread);
	tmrThread=0;

	ec_slave[0].state = EC_STATE_INIT;
    /* request INIT state for all slaves */
    ec_writestate(0);
	
	if (IOmap!=NULL)
	{
		free(IOmap);
		IOmap=NULL;
	}
	
	ec_close();
}
JNIEXPORT void JNICALL Java_soem_SoemInterrop_StopActivity (JNIEnv * jenv, jclass jclasse)
{
	StopActivity();
}
////////////////////////////////////////////////////////////////////////////////////////////////
// for Java return a string with \t separators with Interface name & Interface description
// Interface name should be provided to StartActivity, desciption can be shown to user
JNIEXPORT jstring JNICALL Java_soem_SoemInterrop_GetInterfaces (JNIEnv * jenv, jclass jclasse)
{
	pcap_if_t *alldevs;
    pcap_if_t *d;
	LPSTR buf;   
	jstring ret;

	char errbuf[PCAP_ERRBUF_SIZE];

	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
        ret= (*jenv)->NewStringUTF(jenv, "");
        return ret;
    }

	buf=(LPSTR)malloc(4096); // should be enought
	buf[0]=0;

	for(d= alldevs; d != NULL; d= d->next)
    {
		strcat(buf,d->name);
		strcat(buf,"\t");
		if (d->description)
			strcat(buf,d->description);
		strcat(buf,"\t");
	}

	ret= (*jenv)->NewStringUTF(jenv, buf);
	free(buf);
	return ret;

}