# -*- makefile -*-
# Makefile for Generic Unix with GCC compiler

# Place where to copy RIPC header files
INCS_INSTAL_PATH=/usr/local/include/ripc

#Place where to copy RIPC libraries
LIBS_INSTALL_PATH=/usr/local/lib

#Place where to copy RIPC server and montor executables
BINS_INSTALL_PATH=/usr/local/bin

TFLAGS=-fPIC -pthread 
THRLIBS=
#TFLAGS=-D_REENTRANT -D_THREAD_SAFE
#THRLIBS=-lpthread


STDLIBS = $(THRLIBS)
ifneq (,$(findstring solaris,$(OSTYPE)))
SOCKLIBS = -lnsl -lsocket
else
SOCKLIBS =
endif


CC = g++

INC_PATH=-I../inc -I../inc/internal
CCFLAGS = -c -Wall -O0 -g $(INC_PATH) $(TFLAGS)
SHFLAGS=-shared


LD = $(CC)
LDFLAGS =  -g $(TFLAGS)


AR = ar
ARFLAGS = -cru

ifneq (,$(findstring freebsd,$(OSTYPE)))
RANLIB = ranlib
else
RANLIB = true
endif

COMMON_INCS=../inc/RIPCdef.h ../inc/RIPCBarrier.h ../inc/RIPCEvent.h ../inc/RIPCLock.h ../inc/RIPCMutex.h ../inc/RIPCPrimitive.h ../inc/RIPCQueue.h ../inc/RIPCSemaphore.h ../inc/RIPCSession.h ../inc/RIPCSharedMemory.h ../inc/RIPCFactory.h ../inc/ripc.h ../inc/RIPCObject.h

SOCKET_INCS=../inc/internal/RIPCSocket.h


INTERNAL_INCS=../inc/internal/RIPCCommBuffer.h ../inc/internal/RIPCCriticalSection.h ../inc/internal/RIPCRequest.h ../inc/internal/RIPCResponse.h $(SOCKET_INCS)


CLIENT_INCS=client/RIPCBarrierStub.h client/RIPCEventStub.h client/RIPCLockStub.h client/RIPCMutexStub.h client/RIPCPrimitiveStub.h client/RIPCQueueStub.h client/RIPCSemaphoreStub.h client/RIPCSessionStub.h client/RIPCSharedMemoryStub.h ../inc/RIPCClientFactory.h

COMMON_OBJS=RIPCPrimitive.o RIPCRequest.o RIPCUnixSocket.o

CLIENT_OBJS=RIPCClientFactory.o RIPCEventStub.o RIPCLockStub.o RIPCMutexStub.o RIPCPrimitiveStub.o RIPCQueueStub.o RIPCSemaphoreStub.o RIPCSessionStub.o RIPCSharedMemoryStub.o 

SERVER_INCS=server/RIPCBarrierImpl.h server/RIPCEventImpl.h server/RIPCLockImpl.h server/RIPCMutexImpl.h server/RIPCPrimitiveImpl.h server/RIPCQueueImpl.h server/RIPCSemaphoreImpl.h server/RIPCSessionImpl.h server/RIPCSharedMemoryImpl.h server/RIPCServer.h server/RIPCHashMap.h server/RIPCIoException.h server/RIPCLockObject.h server/RIPCWaitObject.h ../inc/RIPCThread.h  ../inc/RIPCServerFactory.h 

SERVER_OBJS=RIPCServer.o RIPCEventImpl.o RIPCLockImpl.o RIPCMutexImpl.o RIPCPrimitiveImpl.o RIPCQueueImpl.o RIPCSemaphoreImpl.o RIPCSessionImpl.o RIPCSharedMemoryImpl.o RIPCBarrierImpl.o RIPCThread.o RIPCServerFactory.o RIPCWaitObject.o

LIB_PATH=../lib
BIN_PATH=../bin

BINARIES=$(BIN_PATH)/RIPCServerMonitor $(BIN_PATH)/RIPCServer

all: static shared $(BINARIES)

static: $(LIB_PATH)/libripcclient.a $(LIB_PATH)/libripcserver.a
shared: $(LIB_PATH)/libripcclient.so $(LIB_PATH)/libripcserver.so

RIPCPrimitive.o: RIPCPrimitive.cpp $(COMMON_INCS) 
	$(CC) $(CCFLAGS) RIPCPrimitive.cpp

RIPCRequest.o: RIPCRequest.cpp $(COMMON_INCS) 
	$(CC) $(CCFLAGS) RIPCRequest.cpp

RIPCUnixSocket.o: socket/RIPCUnixSocket.cpp $(SOCKET_INCS)
	$(CC) $(CCFLAGS) -Isocket socket/RIPCUnixSocket.cpp

RIPCClientFactory.o: client/RIPCClientFactory.cpp $(CLIENT_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iclient client/RIPCClientFactory.cpp

RIPCEventStub.o: client/RIPCEventStub.cpp $(CLIENT_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iclient  client/RIPCEventStub.cpp

RIPCLockStub.o: client/RIPCLockStub.cpp $(CLIENT_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iclient client/RIPCLockStub.cpp

RIPCMutexStub.o: client/RIPCMutexStub.cpp $(CLIENT_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iclient client/RIPCMutexStub.cpp 

RIPCPrimitiveStub.o: client/RIPCPrimitiveStub.cpp $(CLIENT_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iclient client/RIPCPrimitiveStub.cpp

RIPCQueueStub.o: client/RIPCQueueStub.cpp $(CLIENT_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iclient client/RIPCQueueStub.cpp 

RIPCSemaphoreStub.o: client/RIPCSemaphoreStub.cpp $(CLIENT_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iclient client/RIPCSemaphoreStub.cpp

RIPCSessionStub.o: client/RIPCSessionStub.cpp $(CLIENT_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iclient client/RIPCSessionStub.cpp

RIPCSharedMemoryStub.o: client/RIPCSharedMemoryStub.cpp $(CLIENT_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iclient client/RIPCSharedMemoryStub.cpp

RIPCBarrierStub.o: client/RIPCBarrierStub.cpp $(CLIENT_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iclient client/RIPCBarrierStub.cpp


$(LIB_PATH)/libripcclient.a: $(CLIENT_OBJS) $(COMMON_OBJS)
	rm -f $(LIB_PATH)/libripcclient.a
	$(AR) $(ARFLAGS) $(LIB_PATH)/libripcclient.a $(CLIENT_OBJS) $(COMMON_OBJS)
	$(RANLIB) $(LIB_PATH)/libripcclient.a

$(LIB_PATH)/libripcclient.so: $(CLIENT_OBJS) $(COMMON_OBJS)
	$(CC) $(SHFLAGS) -o $(LIB_PATH)/libripcclient.so $(CLIENT_OBJS) $(COMMON_OBJS)

RIPCServerMonitor.o: client/RIPCServerMonitor.cpp $(CLIENT_INCS) $(COMMON_INCS)
	$(CC) $(CCFLAGS)  client/RIPCServerMonitor.cpp 

$(BIN_PATH)/RIPCServerMonitor: RIPCServerMonitor.o
	$(LD) $(LDFLAGS) -o $(BIN_PATH)/RIPCServerMonitor RIPCServerMonitor.o $(LIB_PATH)/libripcclient.a $(STDLIBS) $(SOCKLIBS)




RIPCServer.o: server/RIPCServer.cpp $(SERVER_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iserver server/RIPCServer.cpp

RIPCServerFactory.o: server/RIPCServerFactory.cpp $(SERVER_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iserver server/RIPCServerFactory.cpp

RIPCEventImpl.o: server/RIPCEventImpl.cpp $(SERVER_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iserver server/RIPCEventImpl.cpp

RIPCLockImpl.o: server/RIPCLockImpl.cpp $(SERVER_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iserver server/RIPCLockImpl.cpp

RIPCMutexImpl.o: server/RIPCMutexImpl.cpp $(SERVER_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iserver server/RIPCMutexImpl.cpp

RIPCPrimitiveImpl.o: server/RIPCPrimitiveImpl.cpp $(SERVER_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iserver server/RIPCPrimitiveImpl.cpp

RIPCQueueImpl.o: server/RIPCQueueImpl.cpp $(SERVER_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iserver server/RIPCQueueImpl.cpp

RIPCSemaphoreImpl.o: server/RIPCSemaphoreImpl.cpp $(SERVER_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iserver server/RIPCSemaphoreImpl.cpp

RIPCSessionImpl.o: server/RIPCSessionImpl.cpp $(SERVER_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iserver server/RIPCSessionImpl.cpp

RIPCSharedMemoryImpl.o: server/RIPCSharedMemoryImpl.cpp $(SERVER_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iserver server/RIPCSharedMemoryImpl.cpp

RIPCBarrierImpl.o: server/RIPCBarrierImpl.cpp $(SERVER_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iserver server/RIPCBarrierImpl.cpp

RIPCWaitObject.o: server/RIPCWaitObject.cpp $(SERVER_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iserver server/RIPCWaitObject.cpp

RIPCThread.o: server/RIPCThread.cpp $(SERVER_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iserver server/RIPCThread.cpp


$(LIB_PATH)/libripcserver.a: $(SERVER_OBJS) $(COMMON_OBJS)
	rm -f $(LIB_PATH)/libripcserver.a
	$(AR) $(ARFLAGS) $(LIB_PATH)/libripcserver.a $(SERVER_OBJS) $(COMMON_OBJS)
	$(RANLIB) $(LIB_PATH)/libripcserver.a

$(LIB_PATH)/libripcserver.so: $(SERVER_OBJS) $(COMMON_OBJS)
	$(CC) $(SHFLAGS) -o $(LIB_PATH)/libripcserver.so $(SERVER_OBJS) $(COMMON_OBJS)

RIPCServerMain.o: server/RIPCServerMain.cpp $(SERVER_INCS) $(COMMON_INCS) $(INTERNAL_INCS)
	$(CC) $(CCFLAGS) -Iserver server/RIPCServerMain.cpp

$(BIN_PATH)/RIPCServer: RIPCServerMain.o $(LIB_PATH)/libripcserver.a
	$(LD) $(LDFLAGS) -o $(BIN_PATH)/RIPCServer RIPCServerMain.o $(LIB_PATH)/libripcserver.a $(STDLIBS) $(SOCKLIBS)

install: all
	mkdir -p $(INCS_INSTAL_PATH)
	mkdir -p $(LIBS_INSTALL_PATH)
	mkdir -p $(BINS_INSTALL_PATH)
	cp ../bin/* $(BINS_INSTALL_PATH)
	cp ../lib/* $(LIBS_INSTALL_PATH)
	cp ../inc/* $(INCS_INSTAL_PATH)

clean:	
	rm -f *.o ../lib/* ../bin/* ../tests/*.o

tgz:	clean
	cd ../..; tar cvzf ripc.tgz ripc
