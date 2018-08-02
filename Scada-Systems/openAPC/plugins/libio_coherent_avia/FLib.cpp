#include "FLib.h"
#include "FException.h"

#include <sstream>
#include <errno.h>
#include <iostream>
#include <iomanip>
#ifdef ENV_LINUX
 #include <dirent.h>
#endif

#if defined (_WIN32)
#  include <windows.h>
#  include <conio.h>
#elif defined (_MACOSX_) || defined (__linux__)
#  include <stdio.h>
#  include <string.h>
#  include <unistd.h>
#  include <termios.h>
#  include <stdlib.h>
#  include <sys/ioctl.h>
#endif

#if defined (_WIN32)
#	if	defined (__GNUC__)
#			include <pthread.h>
#	endif
#	if	! defined (__GNUC__)
#			define snprintf sprintf_s
#	endif
#endif

using namespace std;
using namespace Frewitt;

const char FLib::TAB = '\t';
const char FLib::CR  = '\r';
const char FLib::LF  = '\n';
const unsigned char FLib::FNC1 = (unsigned char)232; // for GS1

// sleep [ms]
void FLib::Sleep(const unsigned int delay) {
#if defined (_WIN32)
	// Name of the function = MySleep, otherwise there will be a confusion with the Windows Sleep
	// in Windows, by default, Sleep has a low resolution (approx. 10 ms)
	// calling timeBeginPeriod allows going down to 1-2 ms
	// require linking to winmm.lib (or libwimm.a with MinGW)
   //    bool isShort = (1 <= delay) and (20 >= delay);
   //	if (isShort) timeBeginPeriod(1);
	::Sleep(delay);
   //	if (isShort) timeEndPeriod(1);
#elif defined (_MACOSX_) || defined (__linux__)
	int error = usleep(delay*1000);
	if (0 != error) {
      stringstream msg;
      msg << "Error number: " << error;
      throw FException ("FLib", "Sleep", msg.str(), __FILE__, __LINE__);
   } // if
#endif
} // Sleep


// remove char in the string
void FLib::Trim(string& text, const char c) {
   if (text.empty()) return;
   // look for char
   size_t found;
   for (;;) {
      found = text.find_last_of(c);
      if (found != string::npos)
         text.erase (found, 1);
      else
         break;
   } // while
} // Trim

// chop string after char
void FLib::Chop(string& text, const char c) {
   if (text.empty()) return;
   // look for char
   size_t found;
   for (;;) {
      found = text.find_last_of(c);
      if (found != string::npos)
         text.erase (found);
      else
         break;
   } // while
} // Chop

void FLib::ChopCR(string& text) {
   Chop(text, CR);
} // ChopCR

void FLib::ChopLF(string& text) {
   Chop(text, LF);
} // ChopLF

void FLib::ChopCRLF(string& text) {
   ChopCR(text);
   ChopLF(text);
} // ChopCRLF

// remove CR at the end (used for communication and file read, to be compatible Win/Unix/Mac)
void FLib::TrimCR(string& text) {
   Trim(text, CR);
} // TrimCR

// remove LF at the end (used for communication and file read, to be compatible Win/Unix/Mac)
void FLib::TrimLF(string& text) {
   Trim(text, LF);
} // TrimLF

void FLib::TrimCRLF(string& text) {
   TrimCR(text);
   TrimLF(text);
} // TrimCRLF

// remove SP in the string (used for communication and file read)
void FLib::TrimSP(string& text) {
   Trim(text, ' ');
} // TrimSP

#if defined (_MACOSX_) || defined (__linux__)


#endif


void FLib::Execute(const string& command) {
	int err = 0;
	err = system(command.c_str());
	if (err) {
      stringstream msg;
      msg << "Error number: " << err << " while executing command: " << command;
      throw FException("FLib", "Execute", msg.str(), __FILE__, __LINE__);
   } // if
} // Execute

#ifdef ENV_LINUX
DIR* FLib::OpenDir(const string& sdir) {
   DIR *dir = opendir (sdir.c_str());
   if (dir == NULL) {
      throw FException("FLib", "OpenDir", strerror(errno), __FILE__, __LINE__);
   }
   return dir;
}

string FLib::ListNextFile(DIR* dir) {
   struct dirent *ent;
   if( ((ent = readdir(dir)) != NULL) )
      return ent->d_name;
   else return "";
}

void FLib::CloseDir(DIR *dir) {
   closedir (dir);
}

list<string> FLib::ListDir(const string& sdir) {
   struct dirent *ent;
   DIR *dir = OpenDir(sdir);
   list<string> filelist;
   if (dir != NULL) {
      while ( ((ent = readdir(dir)) != NULL) ) {
         filelist.push_back(ent->d_name);
      }
   }
   CloseDir(dir);
   return filelist;
}

bool FLib::DirectoryExists(const string& sdir) {
   DIR *dir;
   dir = opendir (sdir.c_str());
   bool res = (dir != NULL);
   closedir (dir);
   return res;
}
#endif // ENV_LINUX
