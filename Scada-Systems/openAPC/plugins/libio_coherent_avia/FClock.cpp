#include "FClock.h"
#include "FException.h"

#include <sstream>
#include <errno.h>
#include <iostream>
#include <iomanip>
#include <time.h>

#ifdef ENV_WINDOWS
 #include "windows.h"
#endif

using namespace std;
using namespace Frewitt;

// return the frequency of the clock counter, in counter/sec
// to get ms, enter FLib::Clock() * 1E3 / MyClocksPerSec()
__int64 FClock::ClocksPerSecond() {
#if defined (_WIN32)
   LARGE_INTEGER frequency;
	if (!QueryPerformanceFrequency(&frequency)) {
      stringstream msg;
		msg << "Error number: " << GetLastError();
		throw FException("FLib", "ClockPerSec", msg.str(), __FILE__, __LINE__);
	} // if
   return frequency.QuadPart;
#elif defined (_MACOSX_) || defined (__linux__)
	// standard Unix resolution: 0.1 ms resolution
   return CLOCKS_PER_SEC;
#endif
} // ClocksPerSecond

__int64 FClock::ClocksPerMillisecond() {
   return ClocksPerSecond() / 1000;
} // ClocksPerMillisecond

__int64 FClock::ClocksPerMicrosecond() {
   return ClocksPerSecond() / 1000000;
} // ClocksPerMicrosecond


void FClock::Reset() {
   mActualTime = time(NULL);
   __int64 clock = 0;
   
#if defined (_WIN32)
   SYSTEMTIME st;
   GetSystemTime(&st);
//   mMillisecond = st.wMilliseconds;
   LARGE_INTEGER counter;
	if (!QueryPerformanceCounter(&counter)) {
      stringstream msg;
		msg << "Error number: " << GetLastError();
		throw FException("FClock", "Reset", msg.str(), __FILE__, __LINE__);
	}
   clock = counter.QuadPart;
#elif defined (_MACOSX_) || defined (__linux__)
	// don't use clock(), not accurate enough
	struct timeval tv;
	int error = gettimeofday(&tv, NULL);
	if (0 != error) {
      stringstream msg;
      msg << "Error number: " << error;
		throw FException("FClock", "Reset", msg.str(), __FILE__, __LINE__);
	} // if
	// convert in clocks unit
	clock = ClocksPerSecond() * (__int64) tv.tv_sec + ClocksPerMicrosecond() * (__int64) tv.tv_usec;
#endif
   
   mClock = clock;
} // Reset

FClock::FClock() {
   try {
		Reset();
   } catch (const FException& e) {
      cerr << "DEBUG: FClock::FClock: " << e.what() << endl;
   } catch (const exception& e) {
      cerr << "DEBUG: FClock::FClock: " << e.what() << endl;
	}
} // FClock



FClock::~FClock() {
} // ~FClock

FClock& FClock::operator = (const FClock& c) {
	// gracefully handle self assignement	
	if (this == &c) return *this;
   mClock = c.mClock;
   mActualTime = c.mActualTime;
   return *this;
} // =

__int64 FClock::Millisecond() const {
   return mClock / ClocksPerMillisecond();
} // Millisecond

FClock FClock::Now() {
   FClock c;
   return c;
} // Now

bool FClock::operator < (const FClock& c) {
   return (mClock < c.mClock);
} // <

bool FClock::operator > (const FClock& c) {
   return (mClock > c.mClock);
} // >

bool FClock::operator == (const FClock& c) {
   return (mClock == c.mClock);
} // ==

bool FClock::operator != (const FClock& c) {
   return ! (*this == c);
} // !=

FClock FClock::operator + (const FClock& c) {
   FClock result;
   result.mClock       = mClock + c.mClock;
   result.mActualTime  = mActualTime + c.mActualTime;
   return result;
} // +

FClock FClock::operator - (const FClock& c) {
   FClock result;
   result.mClock       = mClock - c.mClock;
   result.mActualTime  = mActualTime - c.mActualTime;
   return result;
} // -

// convert in date format (used for log files)
string FClock::Date(const bool short_format) const {
   const unsigned int BUFFER_SIZE = 80;
	char buffer [BUFFER_SIZE];
   stringstream my_date;
	struct tm *timeinfo;
   
   timeinfo=localtime(&mActualTime);
   if (!timeinfo)
      throw FException("FClock", "Date", "error", __FILE__, __LINE__);
   
   if (short_format) {
      strftime(buffer, BUFFER_SIZE, "%Y%m%d",timeinfo);
   } else {
      strftime(buffer, BUFFER_SIZE, "%Y/%m/%d",timeinfo);
   }
   my_date << buffer;
   return my_date.str();
} // Date

// convert in time format (used for log files)
string FClock::Time(const bool short_format) const {
   const unsigned int BUFFER_SIZE = 80;
	char buffer[BUFFER_SIZE];
   stringstream my_time;
	struct tm *timeinfo;
   
   timeinfo=localtime(&mActualTime);
   if (timeinfo)
      throw FException("FLib", "Time", "error", __FILE__, __LINE__);
   
   int millisecond = (mClock / ClocksPerMillisecond()) % 1000;

   if (short_format) {
      strftime(buffer, BUFFER_SIZE, "%H%M", timeinfo);
      my_time << buffer;
   } else	{
      strftime(buffer, BUFFER_SIZE, "%H:%M:%S", timeinfo);
      my_time << buffer << "." << setw(3) << setfill('0') << millisecond;
   }
   return my_time.str();
} // Time

namespace Frewitt {
   ostream& operator << (ostream& output, const FClock& clock) {
      output << clock.Time();
      return output;
   } // <<
}

