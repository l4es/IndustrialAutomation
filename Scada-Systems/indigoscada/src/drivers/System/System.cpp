/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
/*
* System monitoring driver
*/
#include "System.h"
#include <qtextstream.h>
#include "SystemConfigure.h"
#include "IndentedTrace.h"
/*
*Function:System
*Inputs:parent object, object name
*Outputs:none
*Returns:none
*/
System::System(QObject *parent,const QString &name) : Driver(parent,name),pInstance(0)
{
	connect (GetConfigureDb (),
	SIGNAL (TransactionDone (QObject *, const QString &, int, QObject*)), this,
	SLOT (QueryResponse (QObject *, const QString &, int, QObject*)));	// connect to the database
};
/*
*Function:~System
*Inputs:none
*Outputs:none
*Returns:none
*/
System::~System()
{
	Stop(); // clean up
};
/*
*Function:UnitConfigure
*Inputs:parent widget, obejct name, receipe
*Outputs:none
*Returns:none
*/
void System::UnitConfigure(QWidget *parent, const QString &name, const QString &) // configure a unit
{
	// update the properties now
	SystemConfigure dlg(parent,name);
	dlg.exec();   
};
/*
*Function:SetTypeList
*Inputs:combo box, unit name
*Outputs:none
*Returns:none
*/
void System::SetTypeList(QComboBox *pCombo, const QString &unitname) // set the type list for unit type
{
	pCombo->insertItem(TYPE_M_SP_NA_1);
	pCombo->insertItem(TYPE_M_DP_NA_1);
	pCombo->insertItem(TYPE_M_ME_NA_1);
	pCombo->insertItem(TYPE_M_ME_NB_1);
	pCombo->insertItem(TYPE_M_ME_NC_1);
	pCombo->insertItem(TYPE_M_IT_NA_1);
	pCombo->insertItem(TYPE_M_SP_TB_1);
	pCombo->insertItem(TYPE_M_DP_TB_1);
	pCombo->insertItem(TYPE_M_ME_TD_1);
	pCombo->insertItem(TYPE_M_ME_TE_1);
	pCombo->insertItem(TYPE_M_ME_TF_1);
	pCombo->insertItem(TYPE_M_IT_TB_1);
};
/*
*Function:GetInputList
*Inputs:sample type
*Outputs:list of types
*Returns:none
*/
void System::GetInputList(const QString &type, QStringList &list,const QString &unit,const QString &name) // set the permitted input IDs
{
	list.clear();
	list << "01";
};
/*
*Function:GetSpecificConfig
*Inputs:parent widget, spname, sp type
*Outputs:none
*Returns:specific config widget for sample point
*/
QWidget * System::GetSpecificConfig(QWidget *parent, const QString &spname, const QString &sptype) //specific config for sample point of type
{
	return 0;
};
/*
*Function:GetTagList
*Inputs:sample point type
*Outputs:list of tags
*Returns:none
*/
void System::GetTagList(const QString &type, QStringList &list,const QString &unit, const QString &) // returns the permitted tags for a given type for this unit
{
	list.clear();
	list << "DiskUsed" <<"MemoryUsed" << "MonitorMemory" <<"MonitorKernelTime" <<"MonitorUserTime" 
	<< "UIMemory";
};
/*
*Function:CreateNewUnit
*Inputs:parent widget, unit name
*Outputs:none
*Returns:none
*/
void System::CreateNewUnit(QWidget *parent, const QString &name) // create a new unit - quick configure
{
	QString pc = 
	"select * from PROPS where SKEY='SAMPLEPROPS' and IKEY='" + name +"';"; 
	//
	// get the properties SKEY = unit name IKEY = receipe name
	GetConfigureDb()->DoExec(this,pc, tcreateNewUnit, name);
};
//
// 
System *System::pDriver; // only one instance shoudl be created
/*
*Function:Start
*Inputs:none
*Outputs:none
*Returns:none
*/
void System::Start() // start everything under this driver's control
{
	//
	// create instances for each unit - get the receipe qualifier
	// request each to start
	// form the database request transaction
	// post it to the database - the magic is in the response handler
	// get all enabled units
	// 
	// form up the list of enabled units
	pInstance = 0;
	QString cmd = "select * from UNITS where UNITTYPE='System' and NAME in(" + DriverInstance::FormUnitList()+ ");";
	GetConfigureDb()->DoExec(this,cmd,tListUnits);
	//
};
/*
*Function:Stop
*Inputs:none
*Outputs:none
*Returns:none
*/
void System::Stop() // stop everything under this driver's control
{
	// ask each instance to stop 
	if(pInstance)
	{
		pInstance->Stop();
		delete pInstance;
	};
	pInstance = 0;
};
/*
*Function:Command
*Inputs:target name, command
*Outputs:none
*Returns:none
*/
void System::Command(const QString &name ,const QString &command)
{
}; // process a command for a named unit 
/*
*Function:QueryResponse
*Inputs:client object, command, transaction id
*Outputs:none
*Returns:none
*/
void System::QueryResponse (QObject *p, const QString &c, int id, QObject*caller)
{
	if(p != this) return; // is this for us
	switch(id)
	{
		case tListUnits:
		{
			int n = GetConfigureDb()->GetNumberResults();
			if(n > 0)
			{
				if(!pInstance)
				{
					pInstance = new SystemInstance(this,GetConfigureDb()->GetString("NAME"));
					pInstance->Start(); // kick it off 
				};
			};
		};
		break;
		case tcreateNewUnit:
		{
			if(GetConfigureDb()->GetNumberResults() == 0)
			{
				QSTransaction &t = GetConfigureDb()->CurrentTransaction();
				//
				QString cmd = QString("insert into SAMPLE values('") + t.Data1 + 
				QString("','System parameters','") +t.Data1+ 
				QString("','"TYPE_M_ME_TF_1"','%',1,1,'01',0,0,0);");
				// 
				GetConfigureDb()->DoExec(0,cmd,0); // post it off

				QStringList l;
				GetTagList(TYPE_M_ME_TF_1,l,"",""); 
				
				CreateSamplePoint(t.Data1, l,"");
			}
		}
		break;
		default:
		break;
	};
};
//
//
//
// *********************************************************************************
// Driver instance stuff
//
//
/*
*Function:
*Inputs:none
*Outputs:none
*Returns:none
*/
void SystemInstance::Start() 
{
	QString cmd = "select * from UNITS where UNITTYPE='System' and NAME in(" + DriverInstance::FormUnitList()+ ");";
	GetConfigureDb()->DoExec(this,cmd,tListUnits);
};
/*
*Function: Stop
*Inputs:none
*Outputs:none
*Returns:none
*/
void SystemInstance::Stop() // stop everything under this driver's control
{
	pTimer->stop();    
}
/*
*Function:Command
*Inputs:none
*Outputs:none
*Returns:none
*/
void SystemInstance::Command(const QString &,const QString &) // process a command for a named unit 
{
}
//
// clock tick interval in milliseconds
// 
#define LN_TICK (120000) //every 2 minutes
/*
*Function: QueryResponse
*Inputs:none
*Outputs:none
*Returns:none
*/
void SystemInstance::QueryResponse (QObject *p, const QString &c, int id, QObject*caller) // handles database responses
{
	if(p != this) return;
	switch(id)
	{
		case tListUnits:
		{
			//
			// get the properties - need to be receipe aware 
			//
				//QString pc = "select * from PROPS where SKEY='SystemMonitor' and IKEY = '(default)';"; 
	// get the properties SKEY = unit name IKEY = receipe name
	//GetConfigureDb()->DoExec(this,pc,tUnitProperties);

			QString pc = 
			"select * from PROPS where SKEY='SystemMonitor' and (IKEY = '(default)' or IKEY='"
			+ GetReceipeName() + "') order by IKEY desc;"; 
			//
			// get the properties SKEY = unit name IKEY = receipe name
			// 
			GetConfigureDb()->DoExec(this,pc,tUnitProperties);

			
			if(GetReceipeName() == "(default)")
			{
				// get the properties for this receipe  - fall back to default
				QString cmd = "select * from SAMPLE where UNIT='" + GetConfigureDb()->GetString("NAME") + "' and ENABLED=1;"; 
				GetConfigureDb()->DoExec(this,cmd,tList);
			}
			else
			{
				QString cmd = 
				"select * from SAMPLE where UNIT='" + GetConfigureDb()->GetString("NAME") + 
				"' and NAME in (" + DriverInstance::FormSamplePointList() + ");"; 
				// only the enabled sample points
				GetConfigureDb()->DoExec(this,cmd,tList);
			};
		}
		break;
		case tUnitProperties: // properties for the unit / receipe 
		{
			if(GetConfigureDb()->GetNumberResults() > 0)
			{
				QString s = GetConfigureDb()->GetString("DVAL");
				QTextIStream is(&s);
				is >> MinDiskSpace >> Drive;
				//
				//
				// for unix systems
				Drive = "/dev/" + Drive;
				//
				//
				//
			};
		};
		break;
		case tList: // list of sample points controlled by the unit
		{
			int n = GetConfigureDb()->GetNumberResults();
			assert(n == 1);
			Name = GetConfigureDb()->GetString("NAME"); // save the name

			pTimer->start(LN_TICK); // start the timer
		};
		break;
		default:
		break;
	};
};
/*
*Function: Tick
*Inputs:none
*Outputs:none
*Returns:none
*/
#ifndef WIN32
#include "sys/statfs.h"
#endif

void SystemInstance::Tick()
{
	#ifndef WIN32
	struct statfs buf;
	if(!statfs((const char *)(Drive),&buf))
	{
		// do the stat on the device
		int disk_used = buf.f_bfree / (1000000 / buf.f_bsize); // free space in mega bytes
	}
	#endif
	 	
	#ifdef WIN32
	
	//globab disk usage
	double disk_used = get_used_disk_space_percentage("c:\\");
	
	//global and calling process memory
	MEMORYSTATUS lpBuffer;
	lpBuffer.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&lpBuffer);
	
	double memoryLoad = (double)lpBuffer.dwMemoryLoad;
	double MonitorMemory = (double)(lpBuffer.dwTotalVirtual - lpBuffer.dwAvailVirtual);
	if(InitialMemory == 0.0)
	{
		InitialMemory = (double)(lpBuffer.dwTotalVirtual - lpBuffer.dwAvailVirtual);
	}
	
	////calling process time
	LONGLONG    ftCreationTime;
    LONGLONG    ftExitTime;
    LONGLONG    ftKernelTime;
    LONGLONG    ftUserTime;
    LONGLONG    ftCurrentTime;
    LONGLONG    ftElapsedTime;
    SYSTEMTIME  stCurrentTime;

	if(GetProcessTimes(GetCurrentProcess(),
                        (FILETIME *) &ftCreationTime,
                        (FILETIME *) &ftExitTime,
                        (FILETIME *) &ftKernelTime,
                        (FILETIME *) &ftUserTime))
    {
    	GetSystemTime(&stCurrentTime);
		SystemTimeToFileTime(&stCurrentTime,(FILETIME *) &ftCurrentTime);
		ftElapsedTime = (ftCurrentTime - ftCreationTime)/10000;
		ftKernelTime = ftKernelTime/10000;
		ftUserTime = ftUserTime/10000;
	}

	#endif //WIN32

	///////////////send to db///////////////////////////////////

	ValueList.clear();
	
	IECValue disk(taglist[0], &disk_used);
	ValueList.insert(ValueList.end(),disk);

	IECValue mem(taglist[1], &memoryLoad);
	ValueList.insert(ValueList.end(),mem);
	
	double deltaMonmem = 100.0*(MonitorMemory - InitialMemory)/InitialMemory;
	IECValue Monmem(taglist[2], &deltaMonmem);
	ValueList.insert(ValueList.end(), Monmem);
	
	double ElapsedTime = (double)ftElapsedTime;
	double KernelTime = (double)ftKernelTime;
	double UserTime = (double)ftUserTime;

	double f = 100.0*KernelTime/ElapsedTime;

	IECValue kertime(taglist[3], &f);
	ValueList.insert(ValueList.end(),kertime);

	f = 100.0*UserTime/ElapsedTime;
	IECValue usertime(taglist[4], &f);
	ValueList.insert(ValueList.end(),usertime);
	
	IECValue UImem(taglist[5], &ui_mem_data);
	ValueList.insert(ValueList.end(),UImem);

	PostList(Name,ValueList); // post the list of values up

};

/*
*Function:CurrentNotify
*Inputs:notification code
*Outputs:none
*Returns:none
*/
void SystemInstance::ReceivedNotify(int ntf, const char * data)
{
	IT_IT("SystemInstance::ReceivedNotify");
			
	switch(ntf)
	{
		case NotificationEvent::UI_EXE_DATA:
		{
			dispatcher_extra_params* params = (dispatcher_extra_params *) data;
			ui_mem_data = params->value;
		}
		break;
		default:
		break;
	}
};

// ********************************************************************************************************************************
/*
*Function:GetDriverEntry
*Inputs:parent object
*Outputs:none
*Returns:driver interface 
*/
extern "C"
{ 
	#ifdef WIN32
	SYSTEMDRV Driver *  _cdecl _GetDriverEntry(QObject *parent); 
	SYSTEMDRV void _cdecl _Unload();
	#endif

	
	Driver * _cdecl _GetDriverEntry(QObject *parent) 
	{
		if(!System::pDriver )
		{
			System::pDriver = new System(parent,"System");
		};
		return System::pDriver;
	};
	/*
	*Function: _Unload
	*clean up before DLL unload. and QObjects must be deleted or we get a prang
	*Inputs:none
	*Outputs:none
	*Returns:none
	*/
	void _cdecl _Unload()
	{
		if(System::pDriver) delete System::pDriver;
		System::pDriver = 0;
	};
};

#ifdef WIN32

double get_used_disk_space_percentage( const QString& directory_path )
{
   double number_of_bytes_free_on_drive      = 0.0;
   double total_number_of_bytes_on_drive     = 0.0;
   double high_value                         = static_cast< double >( 0xFFFFFFFF );

   high_value++;

   HANDLE find_file_handle = (HANDLE) NULL;

   QString mask( directory_path );

   if ( mask.at( mask.length() - 1 ) != QChar('\\') &&
        mask.at( mask.length() - 1 ) != QChar('/') )
   {
      mask += QChar('/');
   }

   ULARGE_INTEGER number_of_free_bytes_on_disk;
   ULARGE_INTEGER total_number_of_bytes_on_disk;

   if ( GetDiskFreeSpaceEx( mask,
                           &number_of_free_bytes_on_disk,
                           &total_number_of_bytes_on_disk,
                            NULL ) == FALSE )
   {
      return(0);
   }

   number_of_bytes_free_on_drive  = number_of_free_bytes_on_disk.LowPart;
   number_of_bytes_free_on_drive += static_cast< double >( static_cast< double >( number_of_free_bytes_on_disk.HighPart ) * high_value );

   total_number_of_bytes_on_drive  = total_number_of_bytes_on_disk.LowPart;
   total_number_of_bytes_on_drive += static_cast< double >( static_cast< double >( total_number_of_bytes_on_disk.HighPart ) * high_value );

   double percentage = 0.0;

   percentage = (total_number_of_bytes_on_drive - number_of_bytes_free_on_drive) / total_number_of_bytes_on_drive;
   percentage *= 100.0;

   return(percentage);
}

DWORD get_directory_usage_percentage( const QString& directory_path )
{
   
   double number_of_bytes_free_on_drive      = 0.0;
   double number_of_bytes_used_in_directory  = 0.0;
   double high_value                         = static_cast< double >( 0xFFFFFFFF );

   high_value++;

   HANDLE find_file_handle = (HANDLE) NULL;

   QString mask( directory_path );

   if ( mask.at( mask.length() - 1 ) != QChar('\\') &&
        mask.at( mask.length() - 1 ) != QChar('/') )
   {
      mask += QChar('/');
   }

   mask += QString("*.*");

   WIN32_FIND_DATA find_data;

   ZeroMemory( &find_data, sizeof( find_data ) );

   find_file_handle = FindFirstFile( (const char*)mask, &find_data );

   if ( find_file_handle != (HANDLE) INVALID_HANDLE_VALUE )
   {
      number_of_bytes_used_in_directory += find_data.nFileSizeLow;

      if ( find_data.nFileSizeHigh != 0 )
      {
         number_of_bytes_used_in_directory += static_cast< double >( static_cast< double >( find_data.nFileSizeHigh ) * high_value );
      }

      while( FindNextFile( find_file_handle, &find_data ) != FALSE )
      {
         number_of_bytes_used_in_directory += find_data.nFileSizeLow;

         if ( find_data.nFileSizeHigh != 0 )
         {
            number_of_bytes_used_in_directory += static_cast< double >( static_cast< double >( find_data.nFileSizeHigh ) * high_value );
         }
      }

      FindClose( find_file_handle );
   }
   else
   {
      return( 0 );
   }

   // Strip off the "/*.*" from the directory name

   mask = mask.left( mask.length() - 4 );

   ULARGE_INTEGER number_of_free_bytes_on_disk;
   ULARGE_INTEGER total_number_of_bytes_on_disk;

   if ( GetDiskFreeSpaceEx( mask,
                           &number_of_free_bytes_on_disk,
                           &total_number_of_bytes_on_disk,
                            NULL ) == FALSE )
   {
      return(0);
   }

   number_of_bytes_free_on_drive  = number_of_free_bytes_on_disk.LowPart;
   number_of_bytes_free_on_drive += static_cast< double >( static_cast< double >( number_of_free_bytes_on_disk.HighPart ) * high_value );

   double percentage = 0.0;

   percentage = number_of_bytes_used_in_directory / number_of_bytes_free_on_drive;
   percentage *= 100.0;

   return( static_cast< DWORD >( percentage ) );
}


CHAR *lpszArchitecture[]={"INTEL", "MIPS", "ALPHA", "PPC"};

bool FillInSystemInfo(CHAR *buff)
{
    SYSTEM_INFO si;
    CHAR        tmpbuf[1024];

    GetSystemInfo(&si);

//
// Need to actually handle processor level.  
// Just print it for now as a number
//
    wsprintf(tmpbuf,
             "<h2>System Information</h2>\n"
             "<pre>Type Of Machine:      %s</pre>\n"
             "<pre>Level Of Processor:   %d</pre>\n"
             "<pre>Number Of Processors: %d</pre>\n"
             "<hr>\n",
              lpszArchitecture[si.wProcessorArchitecture],
              si.wProcessorLevel,
              si.dwNumberOfProcessors);

    buff = strcat(buff,tmpbuf);
    
    return TRUE;
} // FillInSystemInfo

bool FillInMemoryStatus(CHAR *buff)
{
    MEMORYSTATUS ms;
    CHAR         tmpbuf[1024];

    ms.dwLength = sizeof(MEMORYSTATUS);

    GlobalMemoryStatus(&ms);

    wsprintf(tmpbuf,
             "<h2>Global Memory Status</h2>\n"
             "<pre>Memory Load:               %d%%\n</pre>\n"
             "<pre>Total Physical Memory:     %d MB</pre>\n"
             "<pre>Available Physical Memory: %d MB\n</pre>\n"
             "<pre>Total Page File:           %d MB</pre>\n"
             "<pre>Available Page File:       %d MB\n</pre>\n"
             "<pre>Total Virtual Memory:      %d MB</pre>\n"
             "<pre>Available Virtual Memory:  %d MB</pre>\n"
             "<hr>\n",
             ms.dwMemoryLoad,
             ms.dwTotalPhys/1024/1024,
             ms.dwAvailPhys/1024/1024,
             ms.dwTotalPageFile/1024/1024,
             ms.dwAvailPageFile/1024/1024,
             ms.dwTotalVirtual/1024/1024,
             ms.dwAvailVirtual/1024/1024);
             
    buff = strcat(buff,tmpbuf);

    return TRUE;
} // FillInMemoryStatus

bool FillInProcessInfo(CHAR *buff)
{
    LONGLONG    ftCreationTime;
    LONGLONG    ftExitTime;
    LONGLONG    ftKernelTime;
    LONGLONG    ftUserTime;
    LONGLONG    ftCurrentTime;
    LONGLONG    ftElapsedTime;
    SYSTEMTIME  stCurrentTime;
    CHAR        tmpbuf[1024];
    
    buff = strcat(buff,"<h2>Process Information</h2>\n");

    if(!GetProcessTimes(GetCurrentProcess(),
                        (FILETIME *) &ftCreationTime,
                        (FILETIME *) &ftExitTime,
                        (FILETIME *) &ftKernelTime,
                        (FILETIME *) &ftUserTime))
    {
        buff = strcat(buff,"<p>No Process Information Available\n<hr>\n");
        return TRUE;
    }
                            

    GetSystemTime(&stCurrentTime);
    SystemTimeToFileTime(&stCurrentTime,(FILETIME *) &ftCurrentTime);
    ftElapsedTime = (ftCurrentTime - ftCreationTime)/10000;
    ftKernelTime = ftKernelTime/10000;
    ftUserTime = ftUserTime/10000;

    wsprintf(tmpbuf,
             "ElapsedTime:   %d Day(s)\n"
             "               %d Hr\n"
             "               %d Min\n"
             "               %d Sec\n"
             "               %d mSec\n" 
             "KernelTime:    %d Min\n"
             "               %d Sec\n"
             "               %d mSec\n\n" 
             "UserTime:      %d Min\n"
             "               %d Sec\n"
             "               %d mSec\n"
             "\n",
             (DWORD) (ftElapsedTime/86400000),
             (DWORD) (ftElapsedTime%86400000/3600000),
             (DWORD) (ftElapsedTime%86400000%3600000/60000),
             (DWORD) (ftElapsedTime%86400000%3600000%60000/1000),
             (DWORD) (ftElapsedTime%86400000%3600000%60000%1000),
             (DWORD) (ftKernelTime/60000),
             (DWORD) (ftKernelTime%60000/1000),
             (DWORD) (ftKernelTime%60000%1000),
             (DWORD) (ftUserTime/60000),
             (DWORD) (ftUserTime%60000/1000),
             (DWORD) (ftUserTime%60000%1000));
             
            
    buff = strcat(buff,tmpbuf);
    return TRUE;
} // FillInProcessInfo

#endif //WIN32
