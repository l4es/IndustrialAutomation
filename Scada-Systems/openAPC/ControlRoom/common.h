#ifndef COMMON_H
#define COMMON_H

// common definitions that are used in all parts and with no wxWidgets dependencies

#ifndef ENV_EMBEDDED

#ifdef ENV_WINDOWS
 #pragma pack(push,8)
#endif

#define GLOBAL_VERSION_ID 503011 // <-- DO NOT FORGET!!! Format: Mmmrrr
#ifdef ENV_BEAM
 #define GLOBAL_VERSION wxString(_T("5.3-11 \"River Song\""))
#else
 #ifdef _T
   #define GLOBAL_VERSION wxString(_T("5.3-11 \"River Song\""))
 #else
   #define GLOBAL_VERSION "5.3-11 \"River Song\""
 #endif
#endif

#define GLOBAL_VERSION_ID_LTS 409003 // <-- DO NOT FORGET!!! Format: Mmmrrr
#ifdef ENV_BEAM
 #define GLOBAL_VERSION_LTS wxString(_T("4.9-3 \"Midnight Sun\""))
#else
 #ifdef _T
   #define GLOBAL_VERSION_LTS wxString(_T("4.9-3 \"Midnight Sun\""))
 #else
   #define GLOBAL_VERSION_LTS "4.9-3 \"Midnight Sun\""
 #endif
#endif

#define MAJOR_VERSION  3

#define LICENSE_FILENAME _T(".oapclic")

#ifdef ENV_WINDOWS
 #define DATA_PATH
 #define OEM_PATH ""
 #ifdef _DEBUG
  #ifdef ENV_OAPCPLUGIN
   #define LIB_PATH "..\\CNConstruct\\"
  #else
   #define LIB_PATH ""
  #endif
 #else
  #define LIB_PATH ""
 #endif
 #define MACRO_PATH "macros\\"
#else
 #if defined (ENV_LINUX) || defined (ENV_QNX)
  #include <stdint.h>
  #define DATA_PATH  "/usr/share/openapc/"
  #define OEM_PATH DATA_PATH
  #if __SIZEOF_POINTER__ == 8
   #define LIB_PATH   "/usr/lib64/openapc/"
   #define MACRO_PATH "/usr/lib64/openapc/macros/"
  #else
   #define LIB_PATH   "/usr/lib/openapc/"
   #define MACRO_PATH "/usr/lib/openapc/macros/"
  #endif
 #else
 #error Not implemented!
 #endif
#endif


#ifndef _MSC_VER
 #ifndef ENV_WINDOWSCE
  #include <stdint.h>
 #endif
#endif

#ifdef EXCLUDE_WX
typedef int          wxInt32;
typedef unsigned int wxUint32;
#if defined _MSC_VER | defined ENV_WINDOWSCE
 typedef __int64     wxUint64;
#else
 typedef uint64_t    wxUint64;
#endif
#endif

#define MAX_NAME_LENGTH     128
#define MAX_LIBNAME_LENGTH  150
#define MAX_TEXT_LENGTH     250
#define MAX_PATH_LENGTH     500
#define MAX_LOG_TYPES         4



struct hmiFlowData
{
    wxInt32 version,id,length,usedFlows,maxEdges,flowFlags; // id - the identifier the hmiObject belongs to or the identifier that has to be assigned to the external flowObject
};



struct externalFlowData
{
    struct   hmiFlowData flowData;
    wxInt32  flowX,flowY;
    wxUint32 type,flowFlags;
    wxUint64 userPriviFlags;
    wxUint32 dataFlowFlags;
    wxInt32  cycleMicros;
    wxInt32  extLibDataSize; // size of the data that come from/go to the external lib
    wxInt32  reserved1; // align to 64 bit with this variable
    char     store_name[MAX_NAME_LENGTH*2];
    char     store_libname[MAX_LIBNAME_LENGTH];
};

#define PROJECT_FLAG_ISPACE_RESERVED0     0x0001 // reserved for other ISpace-related mutual excluding options
#define PROJECT_FLAG_ISPACE_SINGLE_LOCAL  0x0002 // one local single ispace server for in- and output data
#define PROJECT_FLAG_ISPACE_DOUBLE_LOCAL  0x0003 // separate local ispace servers for in- and output data and the possibility to have an external interlock for them
#define PROJECT_FLAG_ISPACE_SINGLE_REMOTE 0x0004 // one remote single ispace server for in- and output data
#define PROJECT_FLAG_ISPACE_DOUBLE_REMOTE 0x0005 // separate remote ispace servers for in- and output data and the possibility to have an external interlock for them
#define PROJECT_FLAG_ISPACE_RESERVED1     0x0006 // reserved for other ISpace-related mutual excluding options
#define PROJECT_FLAG_ISPACE_RESERVED2     0x0007 // reserved for other ISpace-related mutual excluding options
#define PROJECT_FLAG_ISPACE_RESERVED3     0x0008 // reserved for other ISpace-related mutual excluding options
#define PROJECT_FLAG_TOUCHSCREEN_SUPPORT  0x0020 // support for touchscreens, enable on-screen keyboard/numpad automatically on selection<<<<<<< common.h
#ifndef PROJECT_FLAG_SNAPTOGRID
 #define PROJECT_FLAG_SNAPTOGRID           0x0040 // snap to grid during drawing
#endif // PROJECT_FLAG_SNAPTOGRID

#define PROJECT_FLAG_ISPACE_MASK          (PROJECT_FLAG_ISPACE_SINGLE_LOCAL|PROJECT_FLAG_ISPACE_DOUBLE_LOCAL|PROJECT_FLAG_ISPACE_SINGLE_REMOTE|PROJECT_FLAG_ISPACE_DOUBLE_REMOTE)



struct hmiObjectFontData
{
    wxInt32  pointSize,style,weight,reserved1;
    char     store_faceName[MAX_NAME_LENGTH*2];
};



struct hmiProjectData
{
   wxInt32  version,reserved1;
   wxInt32  totalW,totalH,gridW,gridH;
   wxInt32  bgCol;
   wxInt32  flags;
   wxInt32  flowW,flowH;
   wxUint32 flowTimeout;    // timeout value in milliseconds after that a control flow thread has to die
   wxUint32 timerResolution; // resolution the timer polls outputs with
   wxInt32  editorX,editorY,editorW,editorH;
   // new since 1.1
   wxInt32                  touchFactor;
   struct hmiObjectFontData font;
   // new since 1.3
   char                     m_remSingleIP[19]; // xxx.xxx.xxx.xxx + 4 chars
   // new since 1.4
   char                     m_remDoubleIP[19]; // xxx.xxx.xxx.xxx + 4 chars
};

#ifdef ENV_WINDOWS
 #pragma pack(pop)
#endif

#endif //ENV_EMBEDDED

#endif

