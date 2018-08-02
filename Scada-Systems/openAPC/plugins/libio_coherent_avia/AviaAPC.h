#ifndef __AVIA_INCLUDED__
#define __AVIA_INCLUDED__

#include <string>

#include "FSocket.h"
#include "libio_coherent_avia.h"
#include "APC.h"

using namespace std;

class Avia {
public:
   Avia(const std::string& host, const unsigned short port);
	~Avia();

	void Load(struct instData *data);

   double Get(const std::string& param) const;

   bool Set_W(const std::string& wr_param, const std::string& rd_param, const double value, const short timeout=PF_TIMEOUT); // set and wait until
   void Set(const std::string& param, const double value);

	void Reset();
	std::string ErrorCode();

	// read
	bool Ready() const;

	// log
	std::string LogLineConsole() const;
	std::string Version() const;
	std::string Status() const;

	bool Temperature_OK() const;
	bool UV_OK(struct instData *data) const;
	bool Current_OK(struct instData *data) const;

	bool Wait_Temperature(const short timeout=PF_TIMEOUT);
	bool Wait_UV(struct instData *data, const short timeout=PF_TIMEOUT);
	bool Wait_Current(struct instData *data, const short timeout=PF_TIMEOUT);
	bool Wait_Thermatrack(const short timeout=PF_TIMEOUT);

	void Search_UV(struct instData *data);
	void Optimize_UV(struct instData *data);

private:
	static const unsigned int PAUSE_WAIT_MS; // in

	Avia();
	Avia(const Avia&);
	Avia& operator = (const Avia&);

	std::string A_Get(const std::string& query) const;
	void A_Set(const std::string& param, const std::string& value);

	void Init();
	bool Wait(const std::string& param, const double value, const short timeout=PF_TIMEOUT);

	Frewitt::TCPSocket* pSocket;
	struct instData *pData;

	std::string error_message (unsigned error);
};
#endif // __AVIA_INCLUDED__
