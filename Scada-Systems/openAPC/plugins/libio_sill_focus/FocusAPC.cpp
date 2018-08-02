#include "FocusAPC.h"
#include "FLib.h"
#include "FException.h"

#ifdef ENV_WINDOWS
 #define snprintf _snprintf
#endif

#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;
using namespace Frewitt;

#ifdef _DEBUG
// #define TEST_MODE // enables test mode where no communication with hardware is done/no hardware is required
#endif

static const unsigned int SLEEP_MS = 500;

#ifdef TEST_MODE
static unsigned int testPos=0;
#endif

Focus::Focus(const string& host, const unsigned short port) {
#ifndef TEST_MODE
	try {
		pSocket = NULL;
		mPosition = 0;
		pSocket = new TCPSocket(host, port);
	} catch (const exception& e) {
		cerr << "DEBUG: Focus::Constructor: " << e.what() << endl;
	}
#endif
} // Focus

Focus::~Focus() {
   if (NULL != pSocket) delete pSocket;
} // ~Focus

void Focus::Set(const string& param, const string& value) {
	try {
		string query = param;
		query.append(value);
		pSocket->write_CR(query);
		string reply=pSocket->readln();
		cout << "Focus::Set: query: " << query << endl;
		cout << "Focus::Set: reply: " << reply << endl;
   } catch (const exception& e) {
		throw FException("Focus", "Set", e.what(), __FILE__, __LINE__);
	}
} // Set

void Focus::Reset() {
#ifndef TEST_MODE
    try {
        pSocket->write_CR("r");
        string reply=pSocket->readln();
        cout << "Focus::Reset " << endl;
        cout << "Focus::Set: reply: " << reply << endl;
    } catch (const exception& e) {
        throw FException("Focus", "Reset", e.what(), __FILE__, __LINE__);
    }
#else
    testPos=0;
#endif
} // Set

string Focus::Get(const string& query) {
	try {
		pSocket->write_CR(query);
		string reply = pSocket->readln();
		FLib::TrimCRLF(reply);
		cout << "Focus::Get: query: " << query << endl;
		cout << "Focus::Get: reply: " << reply << endl;
		return reply;
	} catch (const exception& e) {
		throw FException("Focus", "Get", e.what(), __FILE__, __LINE__);
	}
} // Get

void Focus::SetUnsigned(const string& param, const unsigned value) {
	try {
		stringstream out;
		const int buffer_size = 4;
		char buffer [buffer_size];

		if (value > 999) {
			throw FException("Focus", "SetUnsigned", "value > 999", __FILE__, __LINE__);
		} // if
		snprintf(buffer, buffer_size, "%03d", value);
		out << buffer;
		Set(param, out.str());
	} catch (const exception& e) {
		throw FException("Focus", "SetUnsigned", e.what(), __FILE__, __LINE__);
	}
} // SetUnsigned

unsigned Focus::GetUnsigned(const string& query) {
	try {
		string reply = Get(query);
      
		// erase "<- Offset: "
      size_t found = reply.find(':');
      if (string::npos == found) {
         cout << "DEBUG: Focus::GetUnsigned: can't find a : in the reply string, assuming position = 0" << endl;
//         throw FException("Focus", "GetUnsigned", "can't find a : in the reply string", __FILE__, __LINE__);
         return 0; 
      } // if
		reply.erase(0, found+1);
      if (reply.empty()) {
         cout << "DEBUG: Focus::GetUnsigned: after : , string is emtpy, assuming position = 0" << endl;
//       throw FException("Focus", "GetUnsigned", "empty string after :", __FILE__, __LINE__);
         return 0; 
      } // if
		unsigned i;
		stringstream(reply) >> i;
		return i;
	} catch (const exception& e) {
		throw FException("Focus", "GetUnsigned", e.what(), __FILE__, __LINE__);
	}
} // GetUnsigned

unsigned Focus::Position() {
#ifndef TEST_MODE
	try {
		return GetUnsigned("?f");
	} catch (const exception& e) {
		throw FException("Focus", "Position", e.what(), __FILE__, __LINE__);
	}
#else
    return testPos;
#endif
} // Position

void Focus::SetPosition(const unsigned position) {
#ifndef TEST_MODE
	try {
		// move too far, to allow gap compensation
		SetUnsigned("f+0", position - 20);
		// wait to make sure the movement is done
		FLib::Sleep(SLEEP_MS);
		// move now in the final position, so the gap will be always in the same direction
		SetUnsigned("f+0", position);
		// wait to make sure the movement is done
		FLib::Sleep(SLEEP_MS);
	} catch (const exception& e) {
		throw FException("Focus", "SetPosition", e.what(), __FILE__, __LINE__);
	}
#else
    testPos=position;
#endif
} // SetPosition

ostream& operator << (ostream& output, Focus* f) {
	if (NULL == f) throw FException ("Focus", "<<", "can't print a NULL focus", __FILE__, __LINE__);
	try {
		output << "Focus:" << endl;
		output << "Position: " << f->Position() << endl;
		output << "end of Focus" << endl;
		return output;
	} catch (const exception& e) {
		throw FException("Focus", "<<", e.what(), __FILE__, __LINE__);
	}
} // <<
