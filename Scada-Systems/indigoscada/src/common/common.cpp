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
Header for: Common Databases
Collect common helper routines here
*/
 
#include "common.h"
#include "messages.h"
#include "general_defines.h"
#include <time.h>   //APA added for mktime()
#include "IndentedTrace.h"

typedef std::map<QString,
#ifdef UNIX 
void *, 
#endif 
#ifdef WIN32 
HMODULE, 
#endif 
std::less<QString> > DllDict;
//
DllDict Dlls; // dictionary of Dlls
//
	static QString ReceipeName = "(default)"; // current receipe
	void SetReceipeName(const QString &s) { ReceipeName = s;};
	const QString & GetReceipeName() { return ReceipeName;};

	static QString HomeDirectory = (const char*) 0;
	void SetScadaHomeDirectory(const QString &s) 
	{ 
		#ifdef WIN32
		
		char path[_MAX_PATH];
		
		path[0] = '\0';
		if(GetModuleFileName(NULL, path, _MAX_PATH))
		{
			*(strrchr(path, '\\')) = '\0';        // Strip \\filename.exe off path
			*(strrchr(path, '\\')) = '\0';        // Strip \\bin off path

			HomeDirectory = path;
        }
			
		#else //UNIX

		char path[256];

		strcpy(path, (const char*)s);
		
		*(strrchr(path, '/')) = '\0';        // Strip /filename.exe off path
		*(strrchr(path, '/')) = '\0';        // Strip /bin off path

		HomeDirectory = path;

		#endif
	};

	const QString & GetScadaHomeDirectory() { return HomeDirectory;};

	/*
	*Function: GetDllEntry
	*Inputs:name of dll, name of function
	*Outputs:none
	*Returns:function pointer
	*/
	//
	// we assume on exit the OS frees all libraries
	//
	void * GetDllEntry(const QString &dllName, const QString &funcName)
	{
		IT_IT("GetDllEntry");
		
		void *res = 0;
		#ifdef UNIX		
		void * handle = 0;
		#endif
		#ifdef WIN32
		HMODULE handle = 0;
		#endif
		DllDict::iterator i = Dlls.find(dllName);
		if(!(i == Dlls.end()))
		{
			handle = (*i).second;
		}	
		else
		{ 		 
			handle = 
			#ifdef UNIX
			dlopen((const char *)dllName,RTLD_GLOBAL); // load the library - Linux - add Win32 handling
			#endif
			#ifdef WIN32
			LoadLibrary((const char *)dllName);
			#endif			
			if(handle)
			{
				DllDict::value_type pr(dllName,handle);
				Dlls.insert(pr);
			}
			else
			{
				#ifdef UNIX			
				IT_COMMENT2("Error %ld on Loading: %s",dlerror(), (const char *)dllName);
				#else
				IT_COMMENT2("Error %ld on Loading: %s",GetLastError(),(const char *)dllName);
				#endif
			};
		};
		if(handle)
		{
			#ifdef UNIX		
			res = dlsym(handle,(const char *)funcName);
			#endif
			#ifdef WIN32
			res = (void *)GetProcAddress(handle,(const char *)funcName);
			#endif
		}
		else
		{
			#ifdef UNIX		
			IT_COMMENT1("Error Looking For Entry Point %d", dlerror());
			#else
			IT_COMMENT("Error Looking For Entry Point");
			#endif

			
		};
		return res;
	};
	/*
	*Function:UnloadAllDlls
	*Inputs:none
	*Outputs:none
	*Returns:none
	*/
	void UnloadAllDlls()
	{
		IT_IT("UnloadAllDlls");
		
		DllDict::iterator i = Dlls.begin();
		for(;!(i == Dlls.end()); i++)
		{
			#ifdef UNIX		
			void * res = dlsym((*i).second, DRIVER_EXIT);
			#endif
			#ifdef WIN32
			void * res = (void *)GetProcAddress((*i).second, DRIVER_EXIT);
			#endif
			if(res) // call the exit function - if it is defined
			{
				((FncPtrDriverExit)res)(); 
			};
			#ifdef UNIX
			dlclose((*i).second); // unload it
			#endif
			#ifdef WIN32
			FreeLibrary((*i).second);	
			#endif
		};
		Dlls.clear(); // scrub the dictionary
	};

	//
	//
	/*
	*Function:
	*Inputs:none
	*Outputs:none
	*Returns:none
	*/
	//
	MessageDisplay *MessageDisplay::pMessageWindow = 0;

	void QSMessage(const QString &s)
	{
		if(MessageDisplay::pMessageWindow)
		{
			MessageDisplay::pMessageWindow->Message(s);
		}
		else
		{
			#ifdef UNIX
			cerr << (const char *)s << endl;
			#endif
		};
	}; // global write to message pane or standard error
	void QSTrace(const QString &s)
	{
		if(MessageDisplay::pMessageWindow)
		{
			MessageDisplay::pMessageWindow->Trace(s);
		}
		else
		{
			#ifdef UNIX
			cerr << (const char *)s << endl;
			#endif
		};
	} // global write to trace pane or standard error

	/*
	*Function: Set the Tab Order of a set of fields
	*Inputs:parent window, ull termianted list of child controls to be sorted
	*Outputs:none
	*Returns:none
	*/
	void SetTabOrder(QWidget *Parent, QWidget **pL) // set tab order
	{
		IT_IT("SetTabOrder");
		
		// scan the list
		for(;*pL != 0; pL++)
		{
			if(pL[1])
			{
				QWidget::setTabOrder(pL[0],pL[1]);
			};
		};
	};
	/*
	*Function:GetAlarmStateName
	*translate a state into name
	*Inputs:none
	*Outputs:none
	*Returns:none
	*/
	QString  GetAlarmStateName(int state)
	{
		IT_IT("GetAlarmStateName");
		
		switch(state)
		{
			case OkLevel:
			return QObject::tr("Ok");
			case WarningLevel:
			return QObject::tr("Warning");
			case AlarmLevel:
			return QObject::tr("Alarm");
			case FailureLevel:
			return QObject::tr("Failure");
			default:
			return QObject::tr("None");
		};
		return QString("");
	};
	/*
	*Function:GetAlarmStateBkColour
	*Inputs:alarm level
	*Outputs:none
	*Returns:colour to use
	*/
	QColor GetAlarmStateBkColour(int state)
	{
//		IT_IT("GetAlarmStateBkColour");
		
		switch(state)
		{
			case OkLevel:
			return Qt::green;
			case WarningLevel:
			return Qt::yellow;
			case AlarmLevel:
			return Qt::red;
			case FailureLevel:
			return Qt::blue;
			default:
			return Qt::white;
		};
		return Qt::white;
	};
	/*
	*Function:GetAlarmStateFgColour
	*Inputs:alarm state
	*Outputs:none
	*Returns:colour for text
	*/
	QColor GetAlarmStateFgColour(int state)
	{
//		IT_IT("GetAlarmStateFgColour");
		
		switch(state)
		{
			case OkLevel:
			return Qt::black;
			case WarningLevel:
			return Qt::black;
			case AlarmLevel:
			return Qt::black;
			case FailureLevel:
			return Qt::yellow;
			default:
			return Qt::black;
		};
		return Qt::black;
	};

	/*
	*Function: IsoDateToQDateTime
	*converts an iso date string to a qdatetime
	*Inputs:iso date time (yyyy-mm-dd hh:mm:ss)
	*Outputs:none
	*Returns:qdatetime
	*/
	QDateTime IsoDateQDateTime(const QString &tm)
	{
//		IT_IT("IsoDateQDateTime");
		
		QDateTime res;
		if(tm == "epoch")
		{
			res.setTime_t(0); // set to epoch date time
		}
		else
		{
			char tt[16];
			char dd[16];
			if(sscanf((const char *)tm, "%s%s",dd,tt) == 2)
			{
				dd[4] = 0;
				dd[7] = 0;
				dd[10] = 0;
				tt[2] = tt[5] = tt[8] = 0;
				QDate d(atoi(dd),atoi(dd +5),atoi(dd+8)); // need to handle the time zone bits
				QTime t(atoi(tt),atoi(tt + 3), atoi(tt + 6));
				res = QDateTime(d,t); // build the time
			};
		};        
		return res;
	};

	/*
	*Function: IsoDateMsToQDateTime
	*converts an iso date (with milliseconds) string to a qdatetime
	*Inputs:iso date time (yyyy-mm-dd hh:mm:ss.bbb)
	*Outputs:none
	*Returns:qdatetime
	*/

	QDateTime IsoDateMsToQDateTime(const QString &tm)
	{
//		IT_IT("IsoDateMsToQDateTime");
		
		QDateTime res;

		char tt[16];
		char dd[16];

		if(tm != QString::null)
		{
			if(sscanf((const char *)tm, "%s%s",dd,tt) == 2)
			{
				dd[4] = 0;
				dd[7] = 0;
				dd[10] = 0;
				tt[2] = 0;
				tt[5] = 0;
				tt[8] = 0;
				tt[12] = 0;

				QDate d(atoi(dd),atoi(dd +5),atoi(dd+8));
				QTime t(atoi(tt),atoi(tt + 3), atoi(tt + 6), atoi(tt + 9));
				res = QDateTime(d,t); // build the time
			};

			return res;
		}
		else
		{
			QDate d(0,0,0);
			QTime t(0,0, 0, 0);
			res = QDateTime(d,t); 
			return res;
		}
	};

	/*
	*Function:QDateTimeToIsoDateInSeconds APA added
	*Inputs:date time
	*Outputs:none
	*Returns:string form ready for sql 
	*/
	QString QDateTimeToIsoDateInSeconds(const QDateTime &dt)
	{
		//IT_IT("QDateTimeToIsoDateInSeconds");
		
		QString str;
			
		struct tm t;
		t.tm_year = dt.date().year() > 1900 ? dt.date().year() - 1900 : dt.date().year();
		t.tm_mon = dt.date().month()-1;
		t.tm_mday = dt.date().day();
		t.tm_hour = dt.time().hour();
		t.tm_min = dt.time().minute();
		t.tm_sec = dt.time().second();
		t.tm_isdst = -1; //to force mktime to check for dst
		time_t tempo_in_sec = mktime(&t);
		
		str.sprintf("%ld", tempo_in_sec); 

		return str;
	};

	
	/*
	*Function:QDateTimeToIsoDateInMilliSeconds
	*Inputs:date time
	*Outputs:none
	*Returns:string form ready for sql 
	*/
	QString QDateTimeToIsoDateInMilliSeconds(const QDateTime &dt)
	{
		IT_IT("QDateTimeToIsoDateInMilliSeconds");
		
		QString str;
		struct tm t;
		time_t sec;
		signed __int64 epoc;
		bool daylight_saving_time = 0;

		t.tm_year = dt.date().year() > 1900 ? dt.date().year() - 1900 : dt.date().year();
		t.tm_mon = dt.date().month()-1;
		t.tm_mday = dt.date().day();
		t.tm_hour = dt.time().hour();
		t.tm_min = dt.time().minute();
		t.tm_sec = dt.time().second();
		t.tm_isdst = -1; //to force mktime to check for dst

		sec = mktime(&t);

		//daylight_saving_time = t.tm_isdst > 0 ? 1 : 0;

		//if(daylight_saving_time)
		//{
		//	sec = sec - 3600; //apa tolgo un' ora se siamo in ora legale
		//}

		epoc =  (signed __int64)sec;

		epoc =  epoc*1000 + dt.time().msec();

		char buffer[20];
		_i64toa(epoc, buffer, 10);
		str = QString(buffer);

		IT_COMMENT((const char *)str);

		return str;
	};

	/*
	*Function:ReportFromTime
	*report start time
	*Inputs:none
	*Outputs:none
	*Returns:none
	*/
	QDateTime ReportFromTime(int mode, const QString &Date, const QString &Time)
	{
		IT_IT("ReportFromTime");
		
		QDateTime From = QDateTime::currentDateTime();
		QDateTime t = QDateTime::currentDateTime();
		switch(mode)
		{
			case  LastHour:
			{
				From = t.addSecs(-3600);
			};
			break;
			case  LastFourHours:
			{
				From = t.addSecs(-3600 * 4);
			};
			break;
			case  LastTwelveHours:
			{
				From = t.addSecs(-3600 * 12);
			};
			break;
			case  LastDay:
			{
				From = t.addSecs(-3600 * 24);
			};
			break;
			case  LastWeek:
			{
				From = t.addSecs(-3600 * 24 * 7);
			};
			break;
			case  FromDateTime:
			{
				QString s = Date + " " + Time;
				From = IsoDateQDateTime(s);
			};
			break;
			case  FromToday:
			{
				t = QDateTime(t.date(),QTime(0,0,0));
				From = t.addSecs(QStringToInterval(Time));
			};
			break;
			case  FromYesterday:
			{
				t = QDateTime(t.date(),QTime(0,0,0));
				From = t.addSecs(QStringToInterval(Time) - (3600 * 24)); 
			};
			break;
			default:
			break;
		};
		return From;
	};
	/*
	*Function:ReportToTime
	*Calculate the report end time
	*Inputs:none
	*Outputs:none
	*Returns:none
	*/
	QDateTime ReportToTime(int mode, const QString &Date, const QString &Time)
	{
		IT_IT("ReportToTime");
		
		QDateTime To = QDateTime::currentDateTime();
		QDateTime t  = QDateTime::currentDateTime();
		switch(mode)
		{
			case   Now:
			{
				To = t;
			};
			break;
			case   ToYesterday:
			{
				t = QDateTime(t.date(),QTime(0,0,0));
				To = t.addSecs(QStringToInterval(Time) - (3600 * 24));
			};
			break;
			case   ToToday:
			{
				t = QDateTime(t.date(),QTime(0,0,0));
				To = t.addSecs(QStringToInterval(Time));
			};
			break;
			case   ToDateTime:
			{
				QString s = Date + " " + Time;
				To = IsoDateQDateTime(s);
			};
			break;
			default:
			break;
		};
		return To;
	};
	/*
	*Function:KermitEncodeByte 
	*Inputs:byte
	*Outputs:encoded bytes
	*Returns:none
	*/
	static void KermitEncodeByte(unsigned char b, QString &Out) 
	{                        
		IT_IT("KermitEncodeByte");
		
		if(b < 0x20)
		{
			Out += '{'; 
			Out += (char)(b + 0x20);
		}
		else if((b > 0x7A) && (b < 0x80))
		{
			Out += '|'; 
			Out += (char)(b - 0x5B);
		}
		else if((b < 0xC0) && (b >= 0x80))
		{
			Out += '}'; 
			Out += (char)(b - 0x60);
		}
		else if(b >= 0xC0)
		{
			Out += '~'; 
			Out += (char)(b - 0xA0);
		}
		else
		{
			Out += (char)b;
		};      
	};
	/*
	*Function:KermitEncodePacket
	*Inputs:node address, inpout data, max length
	*Outputs:encoded data
	*Returns:packet length
	*/
	#define STX 2
	#define ETX 3
	int KermitEncodePacket(int Address, const QString &In,QString &Out)
	{
		IT_IT("KermitEncodePacket");
		
		int cs = 0;      
		Out += (char)(STX);
		KermitEncodeByte(Address / 256,Out); 
		cs += Address / 256; 
		KermitEncodeByte(Address & 0xFF,Out);
		cs += Address & 0xFF;
		for(unsigned i = 0; i < In.length(); i++)
		{      
			KermitEncodeByte((unsigned char)(In.constref(i).latin1()),Out);
			cs += (unsigned)(In.constref(i).latin1());
		};
		KermitEncodeByte(cs / 256,Out);
		KermitEncodeByte(cs & 0xFF,Out);
		Out += (char)(ETX);
		return Out.length();
	};
	/*
	*Function: int KermitDecodePacket
	*Inputs:data to decode, length
	*Outputs:decode data, packet address
	*Returns:data length
	*/
	int KermitDecodePacket(const QString &P, QString &O, int &Address)
	{
		IT_IT("KermitDecodePacket");
		
		Address = -1; // default address
		for(int i = 0; P.length(); i++)
		{
			switch(P[i].latin1()) // add escapes as necessary
			{
				case '{':
				{
					i++; O += P.constref(i).latin1() - 0x20;
				};
				break;
				case '}':
				{
					i++; O += P.constref(i).latin1() + 0x60;
				};
				break;
				case '|':
				{
					i++; O += P.constref(i).latin1() + 0x5B;
				};
				break;
				case '~':
				{
					i++; O += P.constref(i).latin1() + 0xA0;
				};
				break;
				case ETX: 
				{     
					int cs = 0;
					// checksum the binary data
					for(unsigned i = 0; i < (O.length() - 2); i++) cs += ((unsigned char)O.constref(i).latin1());    
					//.constref(
					if(cs == (  (unsigned char)(O.constref(O.length() - 1).latin1()) + 256 * 
					(unsigned char)(O.constref(O.length() - 2).latin1()) )  )              
					{
						Address = ((unsigned char)O.constref(0).latin1()) * 256 + (unsigned char )O.constref(1).latin1();      // get the packet address
					};
					return O.length();
				};
				default:
				{
					O += P.constref(i).latin1(); // no transform
				};
				break;
			};    
		};  
		return 0; 
	};

	/*
	*Function: EncodePassword
	*Inputs: string to encode
	*Outputs:none
	*Returns:Encoded password
	*/
	QString  EncodePassword(const QString &s)
	{
		IT_IT("EncodePassword");
		
		//
		// this is a simple and easy to crack password encoder 
		// this just avoids finding out passwords by looking at the database table
		// 
		QString es = s + "p+`$U_AkL%Jgoejhbm"; // right fill with rubbish
		es = es.left(12);
		QString os;
		for(int i = 0; i < 6; i++)
		{
			if(i & 1)
			{
				QChar c = es[i];
				es[i] = es[11 - i]; // simple shuffle
				es[11 - i] = c;
			};
		};
		for(int k = 0; k < 12; k++)
		{
			os += QString::number((es[k].cell() * 11) & 0xFF,19); // encode to base 19
		};
		//
		return os;
	};


#ifdef WIN32

	static  char winverbuf[256];
	static int winver_done=0;

	char *winver()
	{
		char *os_string="Win32";
		if(winver_done)
		{
				return winverbuf;
		}

		OSVERSIONINFOEX osvi;
		BOOL bOsVersionInfoEx;

		// Try calling GetVersionEx using the OSVERSIONINFOEX structure,
		// which is supported on Windows NT versions 5.0 and later.
		// If that fails, try using the OSVERSIONINFO structure,
		// which is supported on earlier versions of Windows and Windows NT

		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		if (! (bOsVersionInfoEx = GetVersionEx ( (OSVERSIONINFO *) &osvi) ) ) {

		// If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.

		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			  return(NULL);
		}

		switch (osvi.dwPlatformId) 
		{

	   case VER_PLATFORM_WIN32_NT:

		  os_string ="WinNT";
		  break;

	   case VER_PLATFORM_WIN32_WINDOWS:

		  if ((osvi.dwMajorVersion > 4) || 
			 ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion > 0)))
				 os_string="Win98";
		  else
				 os_string="Win95";
		  break;

	   case VER_PLATFORM_WIN32s:

		  os_string="Ms Win32s";
		  break;
		}

		sprintf(winverbuf,"%s ver %d.%d (Bld %d)",
				os_string,
				osvi.dwMajorVersion, 
				osvi.dwMinorVersion, 
				osvi.dwBuildNumber & 0xFFFF);
		winver_done=1;
		return winverbuf;
	};
	
	#include <malloc.h>
	BOOL RunningAsAdministrator()
	{
	   BOOL  fAdmin;
	   HANDLE htkThread;
	   TOKEN_GROUPS *ptg = NULL;
	   DWORD cbTokenGroups;
	   DWORD iGroup;
	   SID_IDENTIFIER_AUTHORITY SystemSidAuthority= SECURITY_NT_AUTHORITY;
	   PSID psidAdmin;

	   // This function returns TRUE if the user identifier associated with this
	   // process is a member of the the Administrators group.

	   // First we must open a handle to the access token for this thread.

	   if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, FALSE, &htkThread))
		  if (GetLastError() == ERROR_NO_TOKEN)
		  {
			 // If the thread does not have an access token, we'll examine the
			 // access token associated with the process.

			 if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &htkThread))
			 return FALSE;
		  }
		  else return FALSE;

	   // Then we must query the size of the group information associated with
	   // the token. Note that we expect a FALSE result from GetTokenInformation
	   // because we've given it a NULL buffer. On exit cbTokenGroups will tell
	   // the size of the group information.

	   if (GetTokenInformation(htkThread, TokenGroups, NULL, 0, &cbTokenGroups))
		  return FALSE;

	   // Here we verify that GetTokenInformation failed for lack of a large
	   // enough buffer.

	   if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		  return FALSE;

	   // Now we allocate a buffer for the group information.
	   // Since _alloca allocates on the stack, we don't have
	   // to explicitly deallocate it. That happens automatically
	   // when we exit this function.

	   if (!(ptg= (TOKEN_GROUPS *)_alloca(cbTokenGroups))) return FALSE;

	   // Now we ask for the group information again.
	   // This may fail if an administrator has added this account
	   // to an additional group between our first call to
	   // GetTokenInformation and this one.

	   if (!GetTokenInformation(htkThread, TokenGroups, ptg, cbTokenGroups,
										   &cbTokenGroups
							   )
		  )
		  return FALSE;

	   // Now we must create a System Identifier for the Admin group.

	   if (!AllocateAndInitializeSid
			  (&SystemSidAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
									   DOMAIN_ALIAS_RID_ADMINS,
									   0, 0, 0, 0, 0, 0,
									   &psidAdmin
			  )
		  )
		  return FALSE;

	   // Finally we'll iterate through the list of groups for this access
	   // token looking for a match against the SID we created above.

	   fAdmin= FALSE;

	   for (iGroup= 0; iGroup < ptg->GroupCount; iGroup++)
		  if (EqualSid(ptg->Groups[iGroup].Sid, psidAdmin))
		  {
			 fAdmin= TRUE;

			 break;
		  }

	   // Before we exit we must explicity deallocate the SID
	   // we created.

	   FreeSid(psidAdmin);

	   return(fAdmin);
	}

#endif //WIN32

	QString GetNames()
	{
		QString names = SUPPLIER_NAME_SHORT + QString(" - ") + CUSTOMER_NAME_SHORT;
		return QString(names);
	}
