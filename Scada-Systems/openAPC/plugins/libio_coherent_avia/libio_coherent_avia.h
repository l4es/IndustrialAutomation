#ifndef LIBIO_COHERENT_AVIA_H
#define LIBIO_COHERENT_AVIA_H

#include "oapc_libio.h"
#include "liboapc.h"

#include <queue>

class Avia; // forward declaration

extern lib_oapc_io_callback m_oapc_io_callback;

#define OAPC_AVIA_FLAG_CONTROL_POWER 0x00000001
#define OAPC_AVIA_FLAG_CONTROL_FREQ  0x00000002

#define MAX_STATUSMESSAGE_LENGTH 100

struct libio_config
{
   unsigned short version,length;
   char           m_ip[24];
   unsigned short m_port,res1;
   unsigned char  m_currLimit,m_triggerMode,m_pulseControl,res2;
   unsigned int   m_mPowerLimit,m_mEnergyLimit,m_mCurr,m_flags;
   unsigned short m_thermatrack,m_freq, m_timeout;
   unsigned int   m_mSHGTemp,m_mFHGTemp;
};


struct instData
{
   struct libio_config                config;
   int                                m_callbackID;
   int                                m_prevPower,m_prevFreq;
   bool                               m_running,m_busy;
   unsigned char                      m_prevOn;
   void                              *m_signal;
   struct oapc_bin_head              *m_ctrl;
   Avia                              *m_laserInstance;
   int                                m_errorCode;
   char                               m_statusMessage[MAX_STATUSMESSAGE_LENGTH+4];
};


#endif // LIBIO_COHERENT_AVIA_H
