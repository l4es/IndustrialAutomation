#ifndef __FEXCEPTION_INCLUDED__
#define __FEXCEPTION_INCLUDED__

#include <exception>
#include <string>

namespace Frewitt {
   
   using namespace std;
	
   class FException : public exception {
   public:
      FException(const string& message, const string& file = "", const int line = 0) throw();
      FException(const string& class_, const string& method_, const string& message_, const string& file = "", const int line = 0) throw();
      ~FException() throw();
      const char* what() const throw();
   private:
      string userMessage;
   }; // class
	
} // namespace

#endif // __FEXCEPTION_INCLUDED__
