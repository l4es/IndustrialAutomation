//#define RIPC_IMPLEMENTATION

#include "RIPCThread.h"

#ifdef _WIN32
int RIPCCurrentThread::tlsIndex;
#else
pthread_key_t RIPCCurrentThread::key;
#endif

static RIPCCurrentThread initializer;
