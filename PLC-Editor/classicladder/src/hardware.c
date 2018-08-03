/* Classic Ladder Project */
/* Copyright (C) 2001-2015 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* February 2011 */
/* ------------------ */
/* Hardware Interface */
/* ------------------ */
/* This library is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU Lesser General Public */
/* License as published by the Free Software Foundation; either */
/* version 2.1 of the License, or (at your option) any later version. */

/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU */
/* Lesser General Public License for more details. */

/* You should have received a copy of the GNU Lesser General Public */
/* License along with this library; if not, write to the Free Software */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */


#include "classicladder.h"
#include "global.h"
#include "vars_access.h"
#include "hardware.h"

#ifdef COMEDI_SUPPORT
#ifdef __RTL__
#include <rtl.h>
#include <linux/comedilib.h>
#else
#include <comedilib.h>
#endif
#endif

#if !defined( MODULE )
#include <stdio.h>
#include <unistd.h>
#if defined(X86_IO_ACCESS) && !defined(__WIN32__)
#include <sys/io.h>
#endif
#else
#include <asm/io.h>
#endif
#ifdef RTAI
#include "rtai.h"
#endif

#if defined(__WIN32__) && defined(X86_IO_ACCESS)
#include <windows.h>
#endif

#ifdef RASPBERRY_GPIO_ACCESS
#include <wiringPi.h>
#endif

#ifdef ATMEL_SAM_GPIO_ACCESS
#include "../wiringSam/wiringSam.h"
#endif

#define NBR_COMEDI_DEVICES 4

#ifdef COMEDI_SUPPORT
comedi_t * ComediDev[ NBR_COMEDI_DEVICES ];
#endif


// for DLL "inpout32" to read/write I/O ports addresses under Windows (required since NT) ! */
#if defined(__WIN32__) && defined(X86_IO_ACCESS)
typedef short (_stdcall *inpfuncPtr)(short portaddr);
typedef void (_stdcall *oupfuncPtr)(short portaddr, short datum);
static HINSTANCE hWindowsLibInpout32;
static inpfuncPtr WindowsLibIn;
static oupfuncPtr WindowsLibOut;
void InitWindowsInpout32DLL( void )
{
	/* Load the library */
	hWindowsLibInpout32 = LoadLibrary("inpout32.dll");
	if (hWindowsLibInpout32!=NULL)
	{
		/* Get the addresses of the two read/write functions */
		WindowsLibIn = (inpfuncPtr) GetProcAddress(hWindowsLibInpout32, "Inp32");
		WindowsLibOut = (oupfuncPtr) GetProcAddress(hWindowsLibInpout32, "Out32");
		if ( WindowsLibIn==NULL || WindowsLibOut==NULL )
		{ 
			printf("GetProcAddress for functions in inpout32.dll failed !!!\n");
			hWindowsLibInpout32 = NULL;
		}
	}
	else
	{
		printf("Failed to load Windows library 'inpout32.dll' to read/write i/o ports !!!\n");
	}
}
short  Inpout32DLL_read(short int portaddr)
{
	if ( hWindowsLibInpout32 )
		return (WindowsLibIn)(portaddr);
	return 0;
}
void  Inpout32DLL_write(short int portaddr, short int datum)
{
	if ( hWindowsLibInpout32 )
		(WindowsLibOut)(portaddr,datum);
}
#endif

#ifdef __WIN32__
#define WriteOutport( port, data ) Inpout32DLL_write( port, data )
#define ReadInport( port ) Inpout32DLL_read( port )
#else
#define WriteOutport( port, data ) outb( data, port )
#define ReadInport( port ) inb( port )
#endif

void InitIOConf( )
{
	int NumConf;
	int NbrConf;
	int Pass;
	StrIOConf * pConf;
	for( Pass=0; Pass<2; Pass++)
	{
		NbrConf = (Pass==0)?NBR_INPUTS_CONF:NBR_OUTPUTS_CONF;
		for( NumConf=0; NumConf<NbrConf; NumConf++ )
		{
			pConf = (Pass==0)?&InfosGene->InputsConf[ NumConf ]:&InfosGene->OutputsConf[ NumConf ];
			pConf->FirstClassicLadderIO = -1;
			pConf->DeviceType = DEVICE_TYPE_NONE;
			pConf->SubDevOrAdr = 0;
			pConf->FirstChannel = 0;
			pConf->NbrConsecutivesChannels = 1;
			pConf->FlagInverted = 0;
			pConf->ConfigData = 0;
		}
	}
}

//called in "scan inputs" thread
void ReadPhysicalInputs( void )
{
	int NumConf;
	StrIOConf * pConf;
	unsigned int DatasRead;
	unsigned int Mask;
	unsigned int StartMask;
	int VarNumber;
	for( NumConf=0; NumConf<NBR_INPUTS_CONF; NumConf++ )
	{
		pConf = &InfosGene->InputsConf[ NumConf ];
		/* something mapped ? */
		if ( pConf->FirstClassicLadderIO!=-1 )
		{
			StartMask = 1<<pConf->FirstChannel;
			DatasRead = 0;
			
			/* read the physical inputs */
#ifdef X86_IO_ACCESS
			if ( pConf->DeviceType==DEVICE_TYPE_DIRECT_ACCESS )
			{
				DatasRead = ReadInport( pConf->SubDevOrAdr );
			}
#endif

#ifdef COMEDI_SUPPORT
			if ( pConf->DeviceType>=DEVICE_TYPE_COMEDI )
			{
				int ComediNum = pConf->DeviceType-DEVICE_TYPE_COMEDI;
				/* device is opened ? */
				if ( ComediDev[ ComediNum ] )
				{
					/* try to use the optimized way if possible */
					if ( pConf->FirstChannel<=31 && pConf->NbrConsecutivesChannels+pConf->FirstChannel<=32 )
					{
						comedi_dio_bitfield( ComediDev[ ComediNum ],
									pConf->SubDevOrAdr, 0, &DatasRead );
					}
					else
					{
						int ScanEach;
						int BitValue;
						Mask = 1<<pConf->FirstChannel;
						for( ScanEach=pConf->FirstChannel; ScanEach<pConf->FirstChannel+pConf->NbrConsecutivesChannels; ScanEach++ )
						{
							comedi_dio_read( ComediDev[ ComediNum ],
									pConf->SubDevOrAdr, ScanEach, &BitValue );
							if ( BitValue )
								DatasRead = DatasRead|Mask;
							Mask = Mask<<1;
						}
					}
				}
			}
#endif

#if defined( RASPBERRY_GPIO_ACCESS ) || defined( ATMEL_SAM_GPIO_ACCESS )
			if ( pConf->DeviceType==DEVICE_TYPE_RASPBERRY_GPIO || pConf->DeviceType==DEVICE_TYPE_ATMEL_SAM_GPIO )
			{
				int ScanGpio;
				StartMask = 1;
				Mask = 1;
				for( ScanGpio=pConf->FirstChannel; ScanGpio<pConf->FirstChannel+pConf->NbrConsecutivesChannels; ScanGpio++ )
				{
#ifdef RASPBERRY_GPIO_ACCESS
					if ( digitalRead( ScanGpio ) )
#else
					if ( digitalRead( pConf->SubDevOrAdr, ScanGpio ) )
#endif
						DatasRead = DatasRead|Mask;
					Mask = Mask<<1;
				}
			}
#endif

			if ( pConf->FlagInverted )
				DatasRead = ~DatasRead;

			/* Copy the datas read on the inputs variables */
			Mask = StartMask;
			for (VarNumber=pConf->FirstClassicLadderIO; VarNumber<pConf->FirstClassicLadderIO+pConf->NbrConsecutivesChannels; VarNumber++)
			{
				StrInputFilterAndState * pInput = &InputFilterAndStateArray[ VarNumber ];
				// shift raw buffer
				pInput->BuffRawInput = pInput->BuffRawInput<<1;
				// last state (is on ? if yes, set 1 to last bit buffer)
				if ( DatasRead & Mask )
					pInput->BuffRawInput = pInput->BuffRawInput | 1;
				// filter, is stable "1" ?
				if ( ( pInput->BuffRawInput & pInput->FilterMask ) == pInput->FilterMask )
					pInput->InputStateForLogic =  1;
				// filter, is stable "0" ?
				if ( ( pInput->BuffRawInput & pInput->FilterMask ) == 0 )
					pInput->InputStateForLogic =  0;
					
//////				WriteVar(VAR_PHYS_INPUT,VarNumber,(DatasRead & Mask)?1:0);
				Mask = Mask<<1;
			}
		}
	}


}

// called in "logic" thread to refresh inputs var with filtered state
void CopyInputsStatesToInputsVars( void )
{
	int NumInp;
	for( NumInp=0; NumInp<NBR_PHYS_INPUTS; NumInp++ )
	{
		StrInputFilterAndState * pInput = &InputFilterAndStateArray[ NumInp ];
		// real filtered value? interesting to avoid battle with simul checkboxes clicked by hand!
		if ( pInput->InputStateForLogic!=-1 )
		{
//printf("Write var phys input %d\n",NumInp);
			WriteVar( VAR_PHYS_INPUT,NumInp, pInput->InputStateForLogic );
		}
	}
}

#if defined(NBR_HARD_LIFE_USERS_LEDS)
#ifdef X86_IO_ACCESS
const short int HardLifeUsersLedsPorts[NBR_HARD_LIFE_USERS_LEDS] = HARD_LIFE_USERS_LEDS_PORTS;
const unsigned char HardLifeUsersLedsBits[NBR_HARD_LIFE_USERS_LEDS] = HARD_LIFE_USERS_LEDS_BITS;
const short int HardLifeUsersLedsDirPorts[NBR_HARD_LIFE_USERS_LEDS] = HARD_LIFE_USERS_LEDS_DIR_PORTS;
#endif
#ifdef RASPBERRY_GPIO_ACCESS
const int HardLifeUsersLedsGpio[ NBR_HARD_LIFE_USERS_LEDS ] = HARD_LIFE_USERS_LEDS_GPIO;
#endif
#ifdef ATMEL_SAM_GPIO_ACCESS
const int HardLifeUsersLedsPorts[ NBR_HARD_LIFE_USERS_LEDS ] = HARD_LIFE_USERS_LEDS_PORTS;
const int HardLifeUsersLedsGpio[ NBR_HARD_LIFE_USERS_LEDS ] = HARD_LIFE_USERS_LEDS_GPIO;
#endif
#endif
char GetLifeOrUserLedState( int NumLed )
{
	int State;
	if ( NumLed==0 )
		State = InfosGene->LifeLedState;
	else
		State = ReadVar( VAR_USER_LED, NumLed-1 );
#ifdef HARD_LIFE_USERS_LEDS_CMD_INVERTED
	return State?0:1;
#else
	return State;
#endif
}
void WritePhysicalOutputs( char OnlyForLifeLed )
{
#if defined(NBR_HARD_LIFE_USERS_LEDS) 
	int ScanLed;
	char LifeUserLedAlreadyDoneWithOutputs[ NBR_HARD_LIFE_USERS_LEDS ];
	for( ScanLed=0; ScanLed<NBR_HARD_LIFE_USERS_LEDS; ScanLed++ )
		LifeUserLedAlreadyDoneWithOutputs[ ScanLed ] = FALSE;
#endif
	if ( !OnlyForLifeLed )
	{
		int NumConf;
		StrIOConf * pConf;
		unsigned int DatasToWrite;
		unsigned int Mask;
		int VarNumber;
		for( NumConf=0; NumConf<NBR_OUTPUTS_CONF; NumConf++ )
		{
			pConf = &InfosGene->OutputsConf[ NumConf ];
			/* something mapped ? */
			if ( pConf->FirstClassicLadderIO!=-1 )
			{
				DatasToWrite = 0;
				/* get the datas to write from outputs variables */
				Mask = 1<<pConf->FirstChannel;
#if defined( RASPBERRY_GPIO_ACCESS ) || defined( ATMEL_SAM_GPIO_ACCESS )
				if ( pConf->DeviceType==DEVICE_TYPE_RASPBERRY_GPIO || pConf->DeviceType==DEVICE_TYPE_ATMEL_SAM_GPIO )
					Mask = 1;
#endif
				for (VarNumber=pConf->FirstClassicLadderIO; VarNumber<pConf->FirstClassicLadderIO+pConf->NbrConsecutivesChannels; VarNumber++)
				{
					if ( ReadVar(VAR_PHYS_OUTPUT,VarNumber) )
						DatasToWrite = DatasToWrite | Mask;
					Mask = Mask<<1;
				}
				if ( pConf->FlagInverted )
					DatasToWrite = ~DatasToWrite;
				
				/* write the physical outputs */
#ifdef X86_IO_ACCESS
				if ( pConf->DeviceType==DEVICE_TYPE_DIRECT_ACCESS )
				{
					/* verify if not same register port shared with life/user led bit ? (if yes, do "or") */
#if defined(NBR_HARD_LIFE_USERS_LEDS) 
					for( ScanLed=0; ScanLed<NBR_HARD_LIFE_USERS_LEDS; ScanLed++ )
					{
						if ( pConf->SubDevOrAdr==HardLifeUsersLedsPorts[ScanLed] )
						{
							unsigned char MaskValue = HardLifeUsersLedsBits[ScanLed];
							LifeUserLedAlreadyDoneWithOutputs[ ScanLed ] = TRUE;
							// mask off life/user led bit (usefull if flag "inverted" before... so bit already at "1"!)
							DatasToWrite = DatasToWrite & (~(MaskValue));
							if ( GetLifeOrUserLedState(ScanLed) )
								DatasToWrite = DatasToWrite | (MaskValue);
//printf("life/user led%d, datas=%x\n",ScanLed, DatasToWrite);
						}
					}
#endif
					WriteOutport( pConf->SubDevOrAdr, DatasToWrite ); 
				}
#endif

#ifdef COMEDI_SUPPORT
				if ( pConf->DeviceType>=DEVICE_TYPE_COMEDI )
				{
					int ComediNum = pConf->DeviceType-DEVICE_TYPE_COMEDI;
					/* device is opened ? */
					if ( ComediDev[ ComediNum ] )
					{
						/* try to use the optimized way if possible */
						if ( pConf->FirstChannel<=31 && pConf->NbrConsecutivesChannels+pConf->FirstChannel<=32 )
						{
							int WriteMask = (1<<pConf->NbrConsecutivesChannels) - 1;
							WriteMask = WriteMask<<pConf->FirstChannel;
							comedi_dio_bitfield( ComediDev[ ComediNum ],
										pConf->SubDevOrAdr, WriteMask, &DatasToWrite );
						}
						else
						{
							int ScanEach;
							Mask = 1<<pConf->FirstChannel;
							for( ScanEach=pConf->FirstChannel; ScanEach<pConf->FirstChannel+pConf->NbrConsecutivesChannels; ScanEach++ )
							{
								int BitToWrite = DatasToWrite&Mask;
								comedi_dio_write( ComediDev[ ComediNum ],
									pConf->SubDevOrAdr, ScanEach, BitToWrite?1:0 );
								Mask = Mask<<1;
							}
						}
					}
				}
#endif
#if defined( RASPBERRY_GPIO_ACCESS ) || defined( ATMEL_SAM_GPIO_ACCESS )
				if ( pConf->DeviceType==DEVICE_TYPE_RASPBERRY_GPIO || pConf->DeviceType==DEVICE_TYPE_ATMEL_SAM_GPIO )
				{
					int ScanGpio;
					Mask = 1;
					for( ScanGpio=pConf->FirstChannel; ScanGpio<pConf->FirstChannel+pConf->NbrConsecutivesChannels; ScanGpio++ )
					{
#ifdef RASPBERRY_GPIO_ACCESS
						digitalWrite( ScanGpio, (DatasToWrite&Mask)?HIGH:LOW );
#else
						digitalWrite( pConf->SubDevOrAdr, ScanGpio, (DatasToWrite&Mask)?HIGH:LOW );
#endif
						Mask = Mask<<1;
					}
				}
#endif
			}
		}
	}

#if defined(NBR_HARD_LIFE_USERS_LEDS) 
	for( ScanLed=0; ScanLed<NBR_HARD_LIFE_USERS_LEDS; ScanLed++ )
	{
		/* usefull, if life led shared on a i/o port, and logic "stopped", so outputs not refreshed ! */
		if ( ( OnlyForLifeLed && ScanLed==0 ) || (!OnlyForLifeLed && !LifeUserLedAlreadyDoneWithOutputs[ScanLed]) )
		{
#ifdef X86_IO_ACCESS
			int ScanOtherLed;
			unsigned char MaskValue = 0;
			if ( GetLifeOrUserLedState(ScanLed) )
				MaskValue = HardLifeUsersLedsBits[ScanLed];
			if ( !OnlyForLifeLed )
			{
				// verify if not another led sharing same port...
				for( ScanOtherLed=0; ScanOtherLed<NBR_HARD_LIFE_USERS_LEDS; ScanOtherLed++ )
				{
					if( ScanOtherLed!=ScanLed )
					{
						if ( HardLifeUsersLedsDirPorts[ScanOtherLed]==HardLifeUsersLedsDirPorts[ScanLed] )
						{
							if ( GetLifeOrUserLedState(ScanOtherLed) )
								MaskValue = MaskValue | ( HardLifeUsersLedsBits[ScanOtherLed] );
						}
					}
				}
			}
//printf("life/user leds, led0=%d, led1=%d, datas=%x\n",GetLifeOrUserLedState(0), GetLifeOrUserLedState(1), MaskValue);
			WriteOutport( HardLifeUsersLedsPorts[ScanLed], MaskValue ); 
//printf("write led register... (value=%x)\n",(1<<HARD_LIFE_LED_BIT ));
#endif
#ifdef RASPBERRY_GPIO_ACCESS
			digitalWrite( HardLifeUsersLedsGpio[ ScanLed ], (GetLifeOrUserLedState(ScanLed))?HIGH:LOW );
#endif
#ifdef ATMEL_SAM_GPIO_ACCESS
			digitalWrite( HardLifeUsersLedsPorts[ ScanLed ], HardLifeUsersLedsGpio[ ScanLed ], (GetLifeOrUserLedState(ScanLed))?HIGH:LOW );
#endif
		}
	}
#endif
}

char InitHardware( void )
{
#if defined(__WIN32__) && defined(X86_IO_ACCESS)
	InitWindowsInpout32DLL( );
#endif
#ifdef RASPBERRY_GPIO_ACCESS
	if ( wiringPiSetupGpio( )==-1 )
	{
		printf("Failed to init wiringPi library for RaspberryPI GPIO access !\n");
		return FALSE;
	}
	else
	{
		printf("wiringPi library for RaspberryPI GPIO access inited, BoardRev=%d.\n", piBoardRev() );
	}
#endif
#ifdef ATMEL_SAM_GPIO_ACCESS
	if ( wiringSamSetup( )==-1 )
	{
		printf("Failed to init wiringSam library for Atmel AT91SAM GPIO access !\n");
		return FALSE;
	}
	else
	{
		printf("wiringSam library for Atmel AT91SAM GPIO access inited.\n" );
	}
#endif
	return TRUE;
}
void EndHardware( void )
{
#if defined(__WIN32__) && defined(X86_IO_ACCESS)
	if ( hWindowsLibInpout32 )
		FreeLibrary(hWindowsLibInpout32);
#endif
#ifdef ATMEL_SAM_GPIO_ACCESS
	wiringSamEnd( );
#endif
}

/* for Comedi under RTLinux, is called from init_module( ) and the mask parameter given tell
   which comedi devices to open.
   Under Linux the mask parameter is not used and is taken from the I/O file previously loaded */
void OpenHardware( int ComediToOpenMask )
{
#ifdef COMEDI_SUPPORT
	int ComediNum;
	int Mask;
#ifndef __RTL__
	int Pass;
	/* determine the comedi devices to open from the I/O Mapping Config */
	ComediToOpenMask = 0;
	for( Pass=0; Pass<2; Pass++)
	{
		StrIOConf * pConf;
		int NumConf;
		int NbrConf = (Pass==0)?NBR_INPUTS_CONF:NBR_OUTPUTS_CONF;
		for( NumConf=0; NumConf<NbrConf; NumConf++ )
		{
			pConf = (Pass==0)?&InfosGene->InputsConf[ NumConf ]:&InfosGene->OutputsConf[ NumConf ];
			if ( pConf->FirstClassicLadderIO!=-1 )
			{
				if ( pConf->DeviceType>=DEVICE_TYPE_COMEDI )
					ComediToOpenMask = ComediToOpenMask | (1<<(pConf->DeviceType-DEVICE_TYPE_COMEDI));
			}
		}
	}
#endif

	debug_printf("Open the Comedi hardware...\n");
	/* Open each comedi devices */
	Mask = 0x01;
	for( ComediNum=0; ComediNum<NBR_COMEDI_DEVICES; ComediNum++ )
	{
		ComediDev[ ComediNum ] = 0; /* not opened */
		if ( ComediToOpenMask & Mask )
		{
			char Buffer[ 20 ];
			sprintf( Buffer, "/dev/comedi%d", ComediNum );
			debug_printf( "Opening %s...", Buffer );
			ComediDev[ ComediNum ] = comedi_open( Buffer );
			if ( !ComediDev[ ComediNum ] )
			{
				debug_printf("failed!!!\n");
				sprintf( &InfosGene->ErrorMsgStringToDisplay[1], "ailed to open /dev/comedi%d", ComediNum );
				InfosGene->ErrorMsgStringToDisplay[0] = 'F'; // first char tested in multitask...
			}
			else
			{
				debug_printf("Ok!\n");
			}
		}
		Mask = Mask<<1;
	}
#endif
}

/* Under Linux, call ioperm( ) for the necessary adresses ports to access. */
/* For Comedi, configure the direction of bidirectional lines. */
void ConfigHardware( char ForOutputs )
{
	int NumConf;
	int NbrConf;
	int Pass = ForOutputs;
	StrIOConf * pConf;
	// stop the logic refresh before configuring the following...
	StopRunIfRunning( );
#ifdef MODULE
	debug_printf("Configure the hardware (%s)...\n",ForOutputs?"outputs":"inputs");
#else
	printf("Configure the hardware (%s)...\n",ForOutputs?"outputs":"inputs");
#endif
//////	for( Pass=0; Pass<2; Pass++)
	{
		NbrConf = (Pass==0)?NBR_INPUTS_CONF:NBR_OUTPUTS_CONF;
		for( NumConf=0; NumConf<NbrConf; NumConf++ )
		{
			pConf = (Pass==0)?&InfosGene->InputsConf[ NumConf ]:&InfosGene->OutputsConf[ NumConf ];
//////			if ( pConf->FirstClassicLadderIO!=-1 )
			{
#ifdef X86_IO_ACCESS
				if ( pConf->DeviceType==DEVICE_TYPE_DIRECT_ACCESS || pConf->DeviceType==DEVICE_TYPE_DIRECT_CONFIG )
				{
					char cPortAccessError = FALSE;
#if !defined( MODULE ) && !defined( __WIN32__ )
					printf("Config hardware, ask ioperm 0x%X\n",pConf->SubDevOrAdr);
					if ( ioperm( pConf->SubDevOrAdr, 1, 1 ) )
					{
						printf("!!!Failed in ioperm call for the I/O port : 0x%X (you must be root)\n", pConf->SubDevOrAdr );
						pConf->FirstClassicLadderIO = -1;
						pConf->DeviceType = DEVICE_TYPE_NONE;
						printf("!!!This direct access port has been disabled in the I/O conf to avoid to crash later...\n" );
						sprintf( &InfosGene->ErrorMsgStringToDisplay[1], "ailed in ioperm call for the I/O port (%s) : 0x%X (you must be root)\nIt has been disabled in I/O conf.", (Pass==0)?"inputs":"outputs", pConf->SubDevOrAdr );
						InfosGene->ErrorMsgStringToDisplay[0] = 'F'; // first char tested in multitask...
						cPortAccessError = TRUE;
					}
#endif
					if ( pConf->DeviceType==DEVICE_TYPE_DIRECT_CONFIG && !cPortAccessError )
					{
#if defined(NBR_HARD_LIFE_USERS_LEDS) && defined(HARD_LIFE_USERS_LEDS_DIR_PORTS)
						int ScanLed;
#endif
						int DatasToWrite = pConf->ConfigData;
					/* verify if not same register direction i/o port shared with life led bit ? (if yes, do "or") */
#if defined(NBR_HARD_LIFE_USERS_LEDS) && defined(HARD_LIFE_USERS_LEDS_DIR_PORTS)
						for( ScanLed=0; ScanLed<NBR_HARD_LIFE_USERS_LEDS; ScanLed++ )
						{
							if ( pConf->SubDevOrAdr==HardLifeUsersLedsDirPorts[ScanLed] )
								DatasToWrite = DatasToWrite | (HardLifeUsersLedsBits[ScanLed]);
						}
#endif
						printf("Config hardware, write port 0x%X with config data 0x%X\n",pConf->SubDevOrAdr, DatasToWrite);
						WriteOutport( pConf->SubDevOrAdr, DatasToWrite ); 
					}
				}
#endif

#ifdef COMEDI_SUPPORT
				if ( pConf->DeviceType>=DEVICE_TYPE_COMEDI )
				{
					int ScanEach;
					int ComediNum = pConf->DeviceType-DEVICE_TYPE_COMEDI;
					/* device is opened ? */
					if ( ComediDev[ ComediNum ] )
					{
						for( ScanEach=pConf->FirstChannel; ScanEach<pConf->FirstChannel+pConf->NbrConsecutivesChannels; ScanEach++ )
						{
							comedi_dio_config( ComediDev[ ComediNum ],
									pConf->SubDevOrAdr, ScanEach, (Pass==0)?COMEDI_INPUT:COMEDI_OUTPUT );
						}
					}
				}
#endif

#if defined( RASPBERRY_GPIO_ACCESS ) || defined( ATMEL_SAM_GPIO_ACCESS )
				if ( pConf->DeviceType==DEVICE_TYPE_RASPBERRY_GPIO || pConf->DeviceType==DEVICE_TYPE_ATMEL_SAM_GPIO )
				{
					int ScanGpio;
					for( ScanGpio=pConf->FirstChannel; ScanGpio<pConf->FirstChannel+pConf->NbrConsecutivesChannels; ScanGpio++ )
					{
#ifdef RASPBERRY_GPIO_ACCESS
						pinMode( ScanGpio, (Pass==0)?INPUT:OUTPUT );
						if ( Pass==0 )
							pullUpDnControl( ScanGpio, PUD_UP );
#else
						pinMode( pConf->SubDevOrAdr, ScanGpio, (Pass==0)?INPUT:OUTPUT );
#endif
					}
				}
#endif
			}
		}
	}
	// start the logic now !
	RunBackIfStopped( );
}

void ConfigHardwareForLifeUsersLeds( )
{
#if defined(NBR_HARD_LIFE_USERS_LEDS)

#ifdef X86_IO_ACCESS
#if !defined( MODULE ) && !defined( __WIN32__ )
	int ScanLed,ScanOtherLed;
	for( ScanLed=0; ScanLed<NBR_HARD_LIFE_USERS_LEDS; ScanLed++ )
	{
		unsigned char MaskValue = HardLifeUsersLedsBits[ScanLed];
		printf( "Config hardware for life/user led, ask ioperm 0x%X\n", HardLifeUsersLedsPorts[ScanLed] );
		if ( ioperm( HardLifeUsersLedsPorts[ScanLed], 1, 1 ) )
		{
			printf("!!!Failed in ioperm call for the I/O port : 0x%X (you must be root)\n", HardLifeUsersLedsPorts[ScanLed] );
		}
#if defined( HARD_LIFE_USERS_LEDS_DIR_PORTS )
		// verify if not another led sharing same dir port...
		for( ScanOtherLed=0; ScanOtherLed<NBR_HARD_LIFE_USERS_LEDS; ScanOtherLed++ )
		{
			if( ScanOtherLed!=ScanLed )
			{
				if ( HardLifeUsersLedsDirPorts[ScanOtherLed]==HardLifeUsersLedsDirPorts[ScanLed] )
					MaskValue = MaskValue | ( HardLifeUsersLedsBits[ScanOtherLed] );
			}
		}
		printf( "Config hardware for life/user led (i/o dir), ask ioperm 0x%X + set value: 0x%X\n", HardLifeUsersLedsDirPorts[ScanLed], MaskValue );
		if ( ioperm( HardLifeUsersLedsDirPorts[ScanLed], 1, 1 ) )
		{
			printf("!!!Failed in ioperm call for the I/O port : 0x%X (you must be root)\n", HardLifeUsersLedsDirPorts[ScanLed] );
		}
		else
		{
			outb( MaskValue, HardLifeUsersLedsDirPorts[ScanLed] );
		}
	}
#endif
#endif
#endif

#if defined( RASPBERRY_GPIO_ACCESS ) || defined( ATMEL_SAM_GPIO_ACCESS )
	int ScanLed,ScanOtherLed;
	for( ScanLed=0; ScanLed<NBR_HARD_LIFE_USERS_LEDS; ScanLed++ )
	{
#ifdef RASPBERRY_GPIO_ACCESS
		pinMode( HardLifeUsersLedsGpio[ ScanLed ], OUTPUT );
#else
		pinMode( HardLifeUsersLedsPorts[ ScanLed ], HardLifeUsersLedsGpio[ ScanLed ], OUTPUT );
#endif
	}
#endif

#endif
}

