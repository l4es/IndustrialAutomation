#ifndef __FOCUS_INCLUDED__
#define __FOCUS_INCLUDED__

#include <string>
#include <time.h>

#include "FSocket.h"
#include "libio_sill_focus.h"
#include "APC.h"

using namespace std;

class Focus {
public:
	Focus();
	Focus(const string& host, const unsigned short port);
	~Focus();

	unsigned Position();
	void SetPosition(const unsigned position);
    void Reset();
	friend ostream& operator << (ostream&, Focus* f);
	friend ostream& operator << (ostream&, Focus f);

private:
	Focus(const Focus&);
	Focus& operator = (Focus&);

	void Init();
	
	Frewitt::TCPSocket* pSocket;
	unsigned mPosition;

	string Get(const string& query);
	unsigned GetUnsigned(const string& query);

	void Set(const string& param, const string& value);
	void SetUnsigned(const string& param, const unsigned value);
};
#endif // __FOCUS_INCLUDED__
