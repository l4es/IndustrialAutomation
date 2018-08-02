#ifndef __FCLOCK_INCLUDED__
#define __FCLOCK_INCLUDED__

#include <string>

#if defined (_WIN32)
#  include <time.h>
#elif defined (_MACOSX_) || defined (__linux__)
#  include <sys/time.h>
#endif

#ifdef ENV_LINUX
 #include <sys/types.h>
 typedef int64_t __int64;
#endif

namespace Frewitt {
	
   using namespace std;
	
   class FClock {
   public:
      FClock();
      ~FClock();
      static FClock Now();
      void Reset();
      string Date(const bool short_format = false) const;
      string Time(const bool short_format = false) const;
      __int64 Millisecond() const;
      bool operator < (const FClock&);
      bool operator > (const FClock&);
      bool operator == (const FClock&);
      bool operator != (const FClock&);
      FClock& operator = (const FClock&);
      FClock operator + (const FClock&);
      FClock operator - (const FClock&);
      friend std::ostream& operator << (std::ostream&, const FClock&);

   private:
      static __int64 ClocksPerSecond();
      static __int64 ClocksPerMillisecond();
      static __int64 ClocksPerMicrosecond();
      time_t mActualTime;
      __int64 mClock;
   }; // class
   
	
} // namespace

#endif // __FCLOCK_INCLUDED__
