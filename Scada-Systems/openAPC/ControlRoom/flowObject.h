#ifdef ENV_INT
#include "../flowObject.h"
#endif

#ifndef FLOWOBJECT_H
#define FLOWOBJECT_H

#pragma pack(8)

#include "flowConnection.h"
#include "oapc_libio.h"
#include "liboapc.h"
#include "common.h"

#if defined ENV_PLAYER || ENV_BEAM
 #include "common/oapcBinHead.h"
#endif

class ObjectList; // forward declaration
class flowData;

//#define MAX_DIGI_IOS    MAX_NUM_IOS
//#define MAX_CHAR_IOS    MAX_NUM_IOS

#define MAX_NUM_VALUE  2100000000
#define MIN_NUM_VALUE -2100000000

#define FLOW_TYPE_EXTERNAL_LIB       0x00000100


struct hmiObjectHead
{
   wxUint32 version,size,reserved1,reserved2;
};



// flags:      
struct hmiObjectLogData
{
   wxInt32   mBelowVal,mAboveVal,mFromVal,mToVal;
   wxUint32  flags,reserved1;
};

struct hmiObjectData
{
    wxUint32                    type,hmiFlags,logFlags,id; // align to 64 bit
    wxUint64                    stdIN,stdOUT,userPriviFlags,reserved;
    wxUint32                    dataFlowFlags,isChildOf;
    wxInt32                     mposx,mposy,msizex,msizey;
    wxInt32                     flowposx,flowposy;
    wxUint32                    bgcolor[2],fgcolor[2],state[2];
    wxUint8                     floatAccuracy; // only values 1..4 are used
    wxUint8                     reservedLayoutAlignment;
    wxUint16                    custom3;
    wxInt32                     mMaxValue_off;
    char                        store_name[MAX_NAME_LENGTH*2];
    char                        store_text[2][MAX_TEXT_LENGTH*2];
    struct hmiObjectLogData     log[MAX_LOG_TYPES];
    struct hmiObjectFontData    font[2];
    struct oapc_num_value_block MinValue,MaxValue;
};



struct flowObjectData
{
   struct hmiFlowData                      flowData;
   wxInt32                                 flowX,flowY;
   char                                    store_name[MAX_NAME_LENGTH*2];
};


#define OBJECT_MODE_FLOWELEMENT 1
#define OBJECT_MODE_DEVICE      2

#define FLOW_TYPE_MASK 0x0000FF00

class flowObject : public wxEvtHandler  
{
public:
	flowObject(flowObject *obj);
	virtual ~flowObject();

#define LOG_TYPE_DIGI                 1
#define LOG_TYPE_INTNUM               2
#define LOG_TYPE_FLOATNUM             3
#define LOG_TYPE_CHAR                 4

#define LOG_FLAG_ON_NUM_VALUE_BELOW   0x00000001
#define LOG_FLAG_ON_NUM_VALUE_ABOVE   0x00000002
#define LOG_FLAG_ON_NUM_VALUE_BETWEEN 0x00000004
#define LOG_FLAG_ON_DIGI_VALUE_HI     0x00000008
#define LOG_FLAG_ON_DIGI_VALUE_LO     0x00000010
#define LOG_FLAG_ON_VALUE_CHANGED     0x00000020
#define LOG_FLAG_MASK                 0x0000003F

#define FLAG_MAP_OUTPUT_TO_ISERVER    0x00000001 // send changed output values to iserver
#define FLAG_ALLOW_MOD_FROM_ISERVER   0x00000002 // accept new values from iserver for modifying in/outputs of this object

#ifdef ENV_WINDOWS
 #define FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT 0x0000000100000000
 #define FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT  0x0000000200000000
 #define FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT 0x0000000400000000
 #define FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT  0x0000000800000000
#else
 #define FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT 0x0000000100000000LL
 #define FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT  0x0000000200000000LL
 #define FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT 0x0000000400000000LL
 #define FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT  0x0000000800000000LL
#endif

/*   static wxUint32 LOG_TYPE_INTNUM;
   static wxUint32 LOG_TYPE_FLOATNUM;
   static wxUint32 LOG_TYPE_DIGI;
   static wxUint32 LOG_TYPE_CHAR;

   static wxUint32 LOG_FLAG_ON_NUM_VALUE_BELOW;
   static wxUint32 LOG_FLAG_ON_NUM_VALUE_ABOVE;
   static wxUint32 LOG_FLAG_ON_NUM_VALUE_BETWEEN;
   static wxUint32 LOG_FLAG_ON_DIGI_VALUE_HI;
   static wxUint32 LOG_FLAG_ON_DIGI_VALUE_LO;
   static wxUint32 LOG_FLAG_ON_VALUE_CHANGED;
   static wxUint32 LOG_FLAG_MASK;

   static wxUint32 FLAG_MAP_OUTPUT_TO_ISERVER;
   static wxUint32 FLAG_ALLOW_MOD_FROM_ISERVER;

   static wxUint64 FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT;
   static wxUint64 FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT;
   static wxUint64 FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT;
   static wxUint64 FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT;

*/
   //static wxUint32 FLOW_TYPE_EXTERNAL_LIB;
   static wxUint32 FLOW_TYPE_GROUP;
   static wxUint32 FLOW_TYPE_GROUP_IO_DEFINITION;
   static wxUint32 FLOW_TYPE_CONV_DIGI2NUM;
   static wxUint32 FLOW_TYPE_CONV_DIGI2CHAR;
   static wxUint32 FLOW_TYPE_CONV_NUM2DIGI;
   static wxUint32 FLOW_TYPE_CONV_NUM2BITS;
   static wxUint32 FLOW_TYPE_CONV_NUM2CHAR;
   static wxUint32 FLOW_TYPE_CONV_CHAR2DIGI;
   static wxUint32 FLOW_TYPE_CONV_CHAR2NUM;
   static wxUint32 FLOW_TYPE_CONV_MIXED2CHAR;
   static wxUint32 FLOW_TYPE_CONV_DIGI2PAIR;
   static wxUint32 FLOW_TYPE_CONV_NUM2PAIR;
   static wxUint32 FLOW_TYPE_CONV_CHAR2PAIR;
   static wxUint32 FLOW_TYPE_CONV_PAIR2CHAR;
   static wxUint32 FLOW_TYPE_CONV_PAIR2DIGI;
   static wxUint32 FLOW_TYPE_CONV_PAIR2NUM;
   static wxUint32 FLOW_TYPE_CONV_PAIR2BIN;
   static wxUint32 FLOW_TYPE_CONV_BIN2PAIR;
   static wxUint32 FLOW_TYPE_LOGI_DIGIRSFF;
   static wxUint32 FLOW_TYPE_LOGI_DIGITFF;
   static wxUint32 FLOW_TYPE_LOGI_DIGINOT;
   static wxUint32 FLOW_TYPE_LOGI_NUMNOT;
   static wxUint32 FLOW_TYPE_LOGI_DIGINOP;
   static wxUint32 FLOW_TYPE_LOGI_NUMNOP;
   static wxUint32 FLOW_TYPE_LOGI_CHARNOP;
   static wxUint32 FLOW_TYPE_LOGI_BINNOP;
   static wxUint32 FLOW_TYPE_LOGI_DIGIOR;
   static wxUint32 FLOW_TYPE_LOGI_NUMOR;
   static wxUint32 FLOW_TYPE_LOGI_DIGIAND;
   static wxUint32 FLOW_TYPE_LOGI_NUMAND;
   static wxUint32 FLOW_TYPE_LOGI_DIGIXOR;
   static wxUint32 FLOW_TYPE_LOGI_NUMXOR;
   static wxUint32 FLOW_TYPE_LOGI_NUMADD;
   static wxUint32 FLOW_TYPE_LOGI_NUMSUB;
   static wxUint32 FLOW_TYPE_LOGI_NUMMUL;
   static wxUint32 FLOW_TYPE_LOGI_NUMDIV;
   static wxUint32 FLOW_TYPE_MATH_DIGICMP;
   static wxUint32 FLOW_TYPE_MATH_NUMCMP;
   static wxUint32 FLOW_TYPE_MATH_CHARCMP;
   static wxUint32 FLOW_TYPE_MATH_DIGICTR;
   static wxUint32 FLOW_TYPE_MATH_NUMCTR;
   static wxUint32 FLOW_TYPE_MATH_CHARCTR;
   static wxUint32 FLOW_TYPE_MATH_BINCTR;
   static wxUint32 FLOW_TYPE_FLOW_START;
   static wxUint32 FLOW_TYPE_FLOW_STOP;
   static wxUint32 FLOW_TYPE_FLOW_DELAY;
   static wxUint32 FLOW_TYPE_FLOW_TIMER;
   static wxUint32 FLOW_TYPE_FLOW_NUMTGATE;
   static wxUint32 FLOW_TYPE_FLOW_CHARTGATE;
   static wxUint32 FLOW_TYPE_FLOW_BINTGATE;
   static wxUint32 FLOW_TYPE_FLOW_DIGITGATE;
   static wxUint32 FLOW_TYPE_FLOW_NUMGATE;
   static wxUint32 FLOW_TYPE_FLOW_CHARGATE;
   static wxUint32 FLOW_TYPE_FLOW_BINGATE;
   static wxUint32 FLOW_TYPE_FLOW_DIGIGATE;
   static wxUint32 FLOW_TYPE_MISC_LOG;
   static wxUint32 FLOW_TYPE_MISC_LOG_REC;
   static wxUint32 FLOW_TYPE_MISC_LOGIN;
   static wxUint32 FLOW_TYPE_MISC_ISCONNECT;
   static wxUint32 FLOW_TYPE_LOGI_DIGISHIFT;
   static wxUint32 FLOW_TYPE_LOGI_NUMSHIFT;
   static wxUint32 FLOW_TYPE_LOGI_CHARSHIFT;
   static wxUint32 FLOW_TYPE_LOGI_BINSHIFT;
   static wxUint32 FLOW_TYPE_DIALOGUE;
   static wxUint32 FLOW_TYPE_FILE_DIALOGUE;

   virtual wxUint32        getMode();
   virtual void            setMode(wxUint32 mode);
#ifdef ENV_EDITOR
   virtual wxString        getDefaultName();
   virtual void            doDataFlowDialog(bool hideISConfig);
   virtual void            setFlowPos(ObjectList *parentList,wxPoint pos,wxByte forcePosition,wxByte checkIncoming);
   virtual void            setIncomingConnectionPos(ObjectList *parentList,wxPoint delta);
   virtual wxInt32         getFlowWidth();
   virtual wxInt32         getFlowHeight();

   virtual wxInt32         save(wxFile *FHandle);
   virtual wxInt32         saveDATA(wxFile *FHandle,char chunkName[4],bool isCompiled);
           bool            removeFlowConnection(FlowConnection *connection);
   virtual void            drawFlowConnections(wxAutoBufferedPaintDC *dc,FlowConnection *ignoreFlowCon,wxInt32 x, wxInt32 y);
   virtual void            drawFlowConnections(wxAutoBufferedPaintDC *dc,wxUint32 ignoreOutConnector,wxInt32 x, wxInt32 y);
   virtual FlowConnection *getFlowConnection(wxPoint pos,wxInt32 tolerance);
#else
   virtual void            createNodeNames();
#endif
   virtual wxPoint         getFlowPos();
   virtual void            deleteFlowConnection(FlowConnection *connection);
#ifndef ENV_PLUGGER
   virtual wxInt32         getLoadError();
   virtual wxInt32         load(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset);
   virtual wxInt32         loadDATA(wxFile *FHandle,wxUint32 chunkSize,wxUint32 IDOffset,bool isCompiled);
   virtual wxInt32         loadFlow(wxFile *FHandle,struct hmiFlowData *flowData,wxUint32 IDOffset,bool usesOwnHead,bool isCompiled);
#endif
   virtual void            setData(flowObject *object);
   virtual void            setData(struct hmiObjectData *newdata);
   virtual void            addFlowConnection(FlowConnection *connection);
   virtual wxNode         *getFlowConnection(wxNode *node);
   virtual void            applyData(wxByte all);
#ifdef ENV_PLAYER
#ifndef ENV_PLUGGER
   virtual bool            threadIDOK(wxUint32 *flowThreadID,bool allowSameID);
#endif
   virtual bool            valueHasChanged();
   virtual wxByte          setDigitalInput(FlowConnection *connection,wxByte value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxByte          getDigitalOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual wxByte          setNumInput(FlowConnection *connection,wxFloat64 value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxFloat64       getNumOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput,wxLongLong origCreationTime);
   virtual wxByte          setCharInput(FlowConnection *connection,wxString value,wxUint32 *flowThreadID,flowObject *object);
   virtual wxString        getCharOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
   virtual wxByte          setBinInput(FlowConnection *connection,oapcBinHeadSp &value,wxUint32 *flowThreadID,flowObject *object);
   virtual oapcBinHeadSp   getBinOutput(FlowConnection *connection,wxInt32 *rcode,wxUint64 lastInput);
   virtual void            releaseBinData(FlowConnection *connection);
   virtual wxUint64        getAssignedOutput(wxUint64 input);

   wxLongLong                             lastPollTime;
   bool                                   dataValid[MAX_NUM_IOS+1]; // used during flow processing to read an output only
   wxString                               dataTxt[MAX_NUM_IOS];   // once also if there are several connections
   wxFloat64                              dataNum[MAX_NUM_IOS];   // outgoing from this output
   wxByte                                 dataDigi[MAX_NUM_IOS];
   oapcBinHeadSp                          dataBin[MAX_NUM_IOS];
#endif
   wxByte                                 m_doDestroy;

   struct hmiObjectData data;
   wxString             name;
   wxFloat64            MinValue,MaxValue;
   wxString             text[2],fontFaceName[2];
   wxUint32             m_cycleMicros,m_flowFlags;    // used by external IO lib objects only
#ifdef ENV_EDITOR
   wxBitmap            *flowBitmap;
#else
   char                *nodeNameIn,*nodeNameOut;
#endif

protected:
   void                 checkConnections();

#ifdef ENV_PLAYER
   wxByte               digiBits;
#endif
   wxByte               digi[MAX_NUM_IOS];
   wxList               flowList;
   wxUint32              m_mode;

private:
   flowObjectData       convData;
#ifdef ENV_PLAYER
   wxUint32             lastFlowThreadID;
#else
#endif
};

#endif
