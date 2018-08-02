#ifndef __FSOCKET_INCLUDED__
#define __FSOCKET_INCLUDED__

#include <string>

#ifdef ENV_LINUX
 #include <netinet/in.h>
#endif

#ifdef ENV_WINDOWS
 #include <windows.h>
#endif

#include <ostream>

namespace Frewitt {

   using namespace std;

   class Socket {
   public:
      ~Socket();
      string getLocalAddress(); // TODO: reimplement for UDP
      unsigned short getLocalPort(); // TODO: reimplement for UDP
      void setLocalPort(unsigned short localPort);
      void setLocalAddressAndPort(const string& localAddress, unsigned short localPort = 0);
      static void cleanup();

      static unsigned short resolveService(const string& service, const string& protocol = "tcp");

   private:
      Socket(const Socket& sock);
      Socket& operator = (const Socket& sock);

   protected:
      int sockDesc;
      Socket(int type, int protocol);
      Socket(int sockDesc);
   };

   class CommunicatingSocket : public Socket/*, public std::ostream*/ {
   public:
      virtual void connect(const string& foreignAddress, unsigned short foreignPort);

      bool ready(bool dataReady = false, const long timeout_ms = 0);

      void write(const string& buffer);
      void writeln(const string& buffer);
      void writeln();

      // add a LF at the end of the string
      void write_LF(const string& buffer);

      // add a CR at the end of the string
      void write_CR(const string& buffer);

      // read until LF; remove extra CR if any
      string readln();

      // read actual string; remove extra CR if any
      string read();

      virtual void send(const void* buffer, int bufferLen);
      virtual int receive(void* buffer, int bufferLen);

      virtual string getForeignAddress();
      virtual unsigned short getForeignPort();

      virtual CommunicatingSocket& operator << ( const char* );
      virtual CommunicatingSocket& operator << ( const std::string& );
      virtual CommunicatingSocket& operator >> ( std::string& );

   private:
      static const unsigned int BUFFER_SIZE_MAX;

   protected:
      CommunicatingSocket(int type, int protocol);
      CommunicatingSocket(int newConnSD);
      virtual ~CommunicatingSocket();
      void *sendlock;
      void *recvlock;
   };

   class UDPSocket : public CommunicatingSocket {
   public:
      UDPSocket(unsigned short localPort, unsigned short destPort);
      UDPSocket(unsigned short localPort, const string& foreignAddress, unsigned short foreignPort);

      virtual void connect(const string& foreignAddress, unsigned short foreignPort);
      virtual void send(const void* buffer, int bufferLen);
      virtual int receive(void* buffer, int bufferLen);

   private:
      friend class UDPServerSocket;
      struct sockaddr_in destAddr;
   };

   class TCPSocket : public CommunicatingSocket {
   public:
      TCPSocket();
      TCPSocket(const string& foreignAddress, unsigned short foreignPort);

   private:
      friend class TCPServerSocket;
      TCPSocket(int newConnSD);
   };

   class TCPServerSocket : public Socket {
   public:
      TCPServerSocket(unsigned short localPort, int queueLen = 5);

      TCPServerSocket(const string& localAddress, unsigned short localPort,
                      int queueLen = 5);

      TCPSocket* accept();

   private:
      void setListen(int queueLen);
   }; // class

} // namespace

#endif // __FSOCKET_INCLUDED__
