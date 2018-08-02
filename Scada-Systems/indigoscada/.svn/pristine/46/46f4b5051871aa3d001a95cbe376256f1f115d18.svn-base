/******************  Universal EventLogging utility  ****************
 **                  WinSock Error translation                     **
 *                                                                  *
 *                         Copyright (c) 2000 by Timofei Bondarenko *
 ********************************************************************/
#include <windows.h>
#include <winsock.h>
#include <string.h>
#include <stdlib.h>
#include "unilog.h"

static const char *wse10035_10071[] = {
/*10035*/ "WSAEWOULDBLOCK "       "Resource temporarily unavailable.",
/*10036*/ "WSAEINPROGRESS "       "Operation now in progress.",
/*10037*/ "WSAEALREADY "          "Operation already in progress.",
/*10038*/ "WSAENOTSOCK "          "Socket operation on non-socket.",
/*10039*/ "WSAEDESTADDRREQ "      "Destination address required.",
/*10040*/ "WSAEMSGSIZE "          "Message too long.",
/*10041*/ "WSAEPROTOTYPE "        "Protocol wrong type for socket.",
/*10042*/ "WSAENOPROTOOPT "       "Bad protocol option.",
/*10043*/ "WSAEPROTONOSUPPORT "   "Protocol not supported.",
/*10044*/ "WSAESOCKTNOSUPPORT "   "Socket type not supported.",
/*10045*/ "WSAEOPNOTSUPP "        "Operation not supported.",
/*10046*/ "WSAEPFNOSUPPORT "      "Protocol family not supported.",
/*10047*/ "WSAEAFNOSUPPORT "      "Address family not supported by protocol family.",
/*10048*/ "WSAEADDRINUSE "        "Address already in use.",
/*10049*/ "WSAEADDRNOTAVAIL "     "Cannot assign requested address.",
/*10050*/ "WSAENETDOWN "          "Network is down.",
/*10051*/ "WSAENETUNREACH "       "Network is unreachable.",
/*10052*/ "WSAENETRESET "         "Network dropped connection on reset.",
/*10053*/ "WSAECONNABORTED "      "Software caused connection abort.",
/*10054*/ "WSAECONNRESET "        "Connection reset by peer.",
/*10055*/ "WSAENOBUFS "           "No buffer space available.",
/*10056*/ "WSAEISCONN "           "Socket is already connected.",
/*10057*/ "WSAENOTCONN "          "Socket is not connected.",
/*10058*/ "WSAESHUTDOWN "         "Cannot send after socket shutdown.",
/*10059*/ "WSAETOOMANYREFS "      "Too many references to some kernel object.",
/*10060*/ "WSAETIMEDOUT "         "Connection timed out.",
/*10061*/ "WSAECONNREFUSED "      "Connection refused.",
/*10062*/ "WSAELOOP "             "Cannot translate name.",
/*10063*/ "WSAENAMETOOLONG "      "Name component or name was too long.",
/*10064*/ "WSAEHOSTDOWN "         "Host is down.",
/*10065*/ "WSAEHOSTUNREACH "      "No route to host.",
/*10066*/ "WSAENOTEMPTY "         "Cannot remove a directory that is not empty.",
/*10067*/ "WSAEPROCLIM "          "Too many processes.",
/*10068*/ "WSAEUSERS "            "Ran out of quota.",
/*10069*/ "WSAEDQUOT "            "Ran out of disk quota.",
/*10070*/ "WSAESTALE "            "File handle reference is no longer available.",
/*10071*/ "WSAEREMOTE "           "Item is not available locally."
 };

static const char *wse10091_10093[] = {
/*10091*/ "WSASYSNOTREADY "       "Network subsystem is unavailable.",
/*10092*/ "WSAVERNOTSUPPORTED "   "WINSOCK.DLL version out of range.",
/*10093*/ "WSANOTINITIALISED "    "Successful WSAStartup not yet performed."
};

static const char *wse10101_10112[] = {
/*10101*/ "WSAEDISCON "            "Graceful shutdown in progress.",
/*10102*/ "WSAENOMORE "            "No more results can be returned by WSALookupServiceNext",
/*10103*/ "WSAECANCELLED "         "A call to WSALookupServiceEnd was made.",
/*10104*/ "WSAEINVALIDPROCTABLE "  "Invalid procedure table from service provider.",
/*10105*/ "WSAEINVALIDPROVIDER "   "Invalid service provider version number.",
/*10106*/ "WSAEPROVIDERFAILEDINIT ""Unable to initialize a service provider.",
/*10107*/ "WSASYSCALLFAILURE "     "System call failure.",
/*10108*/ "WSASERVICE_NOT_FOUND "  "No such service is known in the specified name space.",
/*10109*/ "WSATYPE_NOT_FOUND "     "Class type not found.",
/*10110*/ "WSA_E_NO_MORE "         "No more results can be returned by WSALookupServiceNext.",
/*10111*/ "WSA_E_CANCELLED "       "A call to WSALookupServiceEnd was made.",
/*10112*/ "WSAEREFUSED "           "A database query failed because it was actively refused"
 };

static const char *wse11001_11015[] = {
/*11001*/ "WSAHOST_NOT_FOUND "     "Host not found.",
/*11002*/ "WSATRY_AGAIN "          "Non-authoritative host not found.",
/*11003*/ "WSANO_RECOVERY "        "This is a non-recoverable error.",
/*11004*/ "WSANO_DATA "            "Valid name, no data record of requested type.",
/*11005*/ "WSA_QOS_RECEIVERS "         "At least one Reserve has arrived",
/*11006*/ "WSA_QOS_SENDERS "           "At least one Path has arrived",
/*11007*/ "WSA_QOS_NO_SENDERS "        "There are no senders",
/*11008*/ "WSA_QOS_NO_RECEIVERS "      "There are no receivers",
/*11009*/ "WSA_QOS_REQUEST_CONFIRMED " "Reserve has been confirmed",
/*11010*/ "WSA_QOS_ADMISSION_FAILURE " "Error due to lack of resources",
/*11011*/ "WSA_QOS_POLICY_FAILURE "    "Rejected for administrative reasons - bad credentials",
/*11012*/ "WSA_QOS_BAD_STYLE "         "Unknown or conflicting style",
/*11013*/ "WSA_QOS_BAD_OBJECT "        "Problem with some part of the filterspec, or providerspecific buffer in general",
/*11014*/ "WSA_QOS_TRAFFIC_CTRL_ERROR ""Problem with some part of the flowspec",
/*11015*/ "WSA_QOS_GENERIC_ERROR "     "General error" };

struct MSG_DECS { int code; char *str; };
#define MI(x) x, #x " "
static const struct MSG_DECS wse_other[] = {
/*10004*/MI(WSAEINTR)              "Interrupted function call.",
/*10009*/MI(WSAEBADF)              "The file handle supplied is not valid.",
/*10013*/MI(WSAEACCES)             "Permission denied.",
/*10014*/MI(WSAEFAULT)             "Bad address.",
/*10022*/MI(WSAEINVAL)             "Invalid argument.",
/*10024*/MI(WSAEMFILE)             "Too many open files."
#if 0
,
#if 0
/*OS de*/MI(WSAINVALIDPROCTABLE)   "Invalid procedure table from service provider.",
/*OS de*/MI(WSAINVALIDPROVIDER)    "Invalid service provider version number.",
/*OS de*/MI(WSAPROVIDERFAILEDINIT) "Unable to initialize a service provider.",
/*OS de*/MI(WSASYSCALLFAILURE)     "System call failure.",
#endif
/*OS de ! 6*/MI(WSA_INVALID_HANDLE)    "Specified event object handle is invalid.",
/*OS de  87*/MI(WSA_INVALID_PARAMETER) "One or more parameters are invalid.",
/*OS de 996*/MI(WSA_IO_INCOMPLETE)     "Overlapped I/O event object not in signaled state.",
/*OS de 997*/MI(WSA_IO_PENDING)        "Overlapped operations will complete later.",
/*OS de ! 8*/MI(WSA_NOT_ENOUGH_MEMORY) "Insufficient memory available.",
/*OS de 995*/MI(WSA_OPERATION_ABORTED) "Overlapped operation aborted." 
#endif
};


const char *sockerror(int ecode)
{
 if (ecode <= 10093)
   {
    if (ecode >= 10091) return wse10091_10093[ecode - 10091];
    if (ecode <= 10071)
      if (ecode >= 10035) return wse10035_10071[ecode - 10035];
   }
 else if (ecode <= 10112)
   {
    if (ecode >= 10101) return wse10101_10112[ecode - 10101];
   }
 else if (ecode <= 11015)
   {
    if (ecode >= 11001) return wse11001_11015[ecode - 11001];
   }

   {
    int ii;
    for(ii = 0; ii < SIZEOF_ARRAY(wse_other); ii++)
      if (wse_other[ii].code == ecode) return wse_other[ii].str;
   }

 return "<Unknown WSA error>";
}
/* end of sockerr.c */
