#!/usr/bin/env python

######################################################
# Python PdCom example
######################################################

# get the modules
import pdcom
import time, os, select, threading
from socket import socket, AF_INET, SOCK_STREAM

# First you need to derive from pdcom.Process
# and reimplement at least:
#   read()
#   write()
#   flush()
#   connected()
#
# Since communication with the server is asynchronous, the process
# must handle incoming data at any time.
# The easiest way to handle this is to do multithreading. In this example,
# incoming data is processed in a separate thread. This makes it much more
# difficult to synchronize with the main thread, but it is shown here
# how this can be achieved.
class Process(pdcom.Process, threading.Thread):
    def __init__(self, addr):
        # Initialize instances
        pdcom.Process.__init__(self)
        threading.Thread.__init__(self)

        # Save address for later use
        self.__addr = addr

        # Semaphore for thread synchronization
        self.sem = threading.Semaphore(0)

        # Need a pipe to kill secondary thread
        self.__kill = os.pipe()

        # Start thread to handle data from server
        self.start();

        # Wait for connected
        self.sem.acquire()

    def disconnect(self):
        """ Close socket and clean up threads """

        # close the write end of the pipe, making the read end readable
        os.close(self.__kill[1])
        self.join();

    def run(self):
        """
        Reimplemented from threading.Thread (mandatory)
        """

        # Create sockets and connect to server
        self.__socket = socket(AF_INET, SOCK_STREAM)
        self.__socket.connect(self.__addr)

        # Duplicate the write channel to use buffered IO
        self.__fd = os.fdopen(self.__socket.fileno(), "wb")

        while True:
            # Wait until either the socket or the kill pipe
            # becomes readable
            r = select.select([self.__socket, self.__kill[0]],[],[])[0]

            # Check whether the kill pipe is readable
            if self.__kill[0] in r:
                # Process was killed. Exit
                break

            # Continue running, call pdcom.Process.asyncData()
            # to process incoming data. If it returns <= 0, exit
            if self.asyncData() <= 0:
                break

        # kill buffered IO, which also closes the socket
        del self.__fd

    def read(self, n):
        """
        Reimplemented from pdcom.Process (mandatory)
        Fetch data from input stream
        """

        return self.__socket.recv(n)

    def write(self, buf):
        """
        Reimplemented from pdcom.Process (mandatory)

        Send buf to output stream
        Return number of bytes written, which MUST be the whole buffer

        It is wise to use buffered output because the the handler calls
        write() very often.
        """

        # write to buffered IO
        self.__fd.write(buf)
        return len(buf)

    def flush(self):
        """
        Reimplemented from pdcom.Process (mandatory)

        Flush output stream.
        Return 0 on success
        """

        self.__fd.flush()
        return 0    # Flush is always successful

    def connected(self):
        """
        Reimplemented from pdcom.Process (mandatory)

        This signal is received when protocol initialization has completed
        """

        # in __init__, the main thread is waiting on this semaphore
        self.sem.release()

    def listReply(self, variables, directories):
        """
        Reimplemented from pdcom.Process to receive the reply to
        a list request
        """

        print "Variables:", ", ".join([v.path for v in variables])
        print "Directories:", ", ".join(directories)
        self.sem.release()

p = Process(("schleptop",2345))

p.list("/")
p.sem.acquire()

class Subscriber(pdcom.Subscriber):
    def __init__(self):
        pdcom.Subscriber.__init__(self)

        self.sem = threading.Semaphore(0)
        self.__subscription = []

    def newGroupValue(self, time):
        print "newGroupValue", ", ".join(
                [str(s.getValue()) for s in self.__subscription])

    def newValue(self, subscription):
        print "newValue", subscription.getValue()

    def active(self, path, subscription):
        print path, "is active"
        self.__subscription.append(subscription)
        self.sem.release()

    def invalid(self, path, id):
        print path, "is invalid"
        self.sem.release()

s = Subscriber()
p.subscribe(s, "/Logical Operator1", 0.1, 0)
p.subscribe(s, "/ResetMessages/Trigger", 0, 0)
s.sem.acquire()
s.sem.acquire()

time.sleep(1)

# Have to delete subscriber manually before disconnecting from server
del s
p.disconnect()

