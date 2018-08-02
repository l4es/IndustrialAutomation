#include "FException.h"

#include <sstream>
#include <iostream>

using namespace std;
using namespace Frewitt;

FException::FException(const string& class_, const string& method_, const string& message, const string& file, const int line)
throw() : userMessage(message) {
   stringstream s;
	s << "Exception: " << class_ << "::" << method_ << ": " << message;
	if (! file.empty())
      s << endl << ", File: " << file;
	if (0 != line)
      s << endl << ", Line: " << line;
   userMessage = s.str();
} // FException

FException::FException(const string& message, const string& file, const int line)
throw() : userMessage(message) {
   stringstream s;
	s << "Exception: " << message;
	if (! file.empty())
      s << endl << ", File: " << file;
	if (0 != line)
      s << endl << ", Line: " << line;
   userMessage = s.str();
   cerr << userMessage << endl;
} // FException

FException::~FException() throw () {
} // destructor

const char* FException::what() const throw() {
	return userMessage.c_str();
}

