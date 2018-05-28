/*****************************************************************************
 *
 * $Id$
 *
 ****************************************************************************/

#include <string>
#include <stdexcept>

#include <yaml.h>
#include <sqlite3.h>

#include <pdcom.h>
#include <time.h>

#include "../common/Process.h"

#include "Variable.h"

/****************************************************************************/

class Process:
    public Common::Process,
    public PdCom::Process,
    public PdCom::Subscriber
{
    public:
        Process();
        ~Process();

        void setLogin(const char *user) { login = user; }
        void restore();
        void run();
        void signal(int);
        void createPidFile(const std::string &);

    protected:
        bool finished;

        void processConfig(const Yaml &, const std::string &, uint16_t,
                const std::string &); // virtual from Common::Process

    private:
        std::string restorePath;
        int reconnectPeriod;
        int keepalivePeriod;
        int receiveTimeout;
        std::string login;
        int fd;
        bool writeRequest;
        struct timeval lastConnectTime;
        struct timeval keepaliveTime;
        struct timeval lastReceiveTime;
        std::string pidPath;

        PdCom::Variable *restoreVar;
        uint8_t restoreData;
        bool restoreArmed;

        void connect();
        void disconnect();
        void readSocket();
        void writeSocket();

        // Common::Process
        Common::Variable *createVariable(sqlite3 *, const Yaml &);

        // PdCom::Process
        void sendRequest();
        int sendData(const char *, size_t);
        bool clientInteraction(const std::string &, const std::string &,
                const std::string &, std::list<ClientInteraction> &);
        void sigConnected();
        void processMessage(const PdCom::Time &, LogLevel_t, unsigned int,
                const std::string &) const;
        void protocolLog(LogLevel_t, const std::string &) const;

        // PdCom::Subscriber
        void notify(PdCom::Variable *);
        void notifyDelete(PdCom::Variable *);

        void log(int, const std::string &);

        void removePidFile();
};

/****************************************************************************/
