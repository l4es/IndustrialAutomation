#ifndef LIBIO_SILL_FOCUS_H
#define LIBIO_SILL_FOCUS_H

#include "FocusAPC.h"

class Focus; //forward declaration

struct axis_config
{
    int            llimit,hlimit,ufactor;
    unsigned short homeTimeout,res1;
};


struct libio_config
{
    unsigned short      version,length;
    char                m_ip[24];
    unsigned short      m_port,res1;
    struct axis_config  axisConfig;
};



struct axis_runconfig
{
    bool          doHome;
    int           newPos,currPos,newSpeed,currentAxisSpeed;
    bool          targetPosReached;
};



struct instData
{
    struct libio_config                config;
    struct axis_runconfig              runconfig;
    int                                m_callbackID;
    bool                               m_isInitialized,m_running;
    unsigned char                      m_busy;
    struct oapc_bin_head              *m_bin;
    int                                m_acc,m_dec;
    Focus                             *m_focusInstance;
};


#endif //LIBIO_SILL_FOCUS_H
