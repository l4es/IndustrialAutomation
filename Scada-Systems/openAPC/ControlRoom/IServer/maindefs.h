#ifndef MAINDEFS_H
#define MAINDEFS_H

#define FCOMMON_NAME      "Open Interlock Server"

#ifdef ENV_LINUX
#define ENV_POSIX
#endif

#ifdef ENV_QNX
#define ENV_POSIX
#endif

#ifdef ENV_SPARCSOLARIS
#define ENV_POSIX
#endif

#define S_SYSTEM          "/system/"         // system node identifier
#define S_SYSTEM_UPTIME   "/system/uptime"   // NUM0: server uptime in seconds
#define S_SYSTEM_VERSION  "/system/version"  // NUM0: server version number
#define S_SYSTEM_STATE    "/system/state"    // several internal state information,
#define S_SYSTEM_EXIT     "/system/exit"     // exit all connected applications on DIGI0==1
#define S_SYSTEM_ERROR    "/system/error"    // global error information that has to be displayed to the user:
                                             // CHAR0 - title, CHAR1 - error text, NUM2 - message type 
                                             // (0 - none, 1 - error, 2 - warning, 3 - information)
#define S_SYSTEM_PING     "/system/ping"     // empty data node, it can be used to check if the server is still
                                             // alife, messages sent to this node are not forwarded to anybody
#define S_SYSTEM_LOGDATA  "/system/logdata"  // system data node to transfer logging information to the player
#define S_SYSTEM_LASTAUTH "/system/lastauth" // authentication data for this connection

#endif
