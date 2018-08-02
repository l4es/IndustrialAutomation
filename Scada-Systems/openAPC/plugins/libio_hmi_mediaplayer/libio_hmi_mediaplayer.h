#ifndef LIBIO_HMI_MEDIAPLAYER_H
#define LIBIO_HMI_MEDIAPLAYER_H

#define MAX_FILENAME_LEN 252

#pragma pack(8)

class mpWindow;
class EventHandler;

struct configData
{
    unsigned short version,length;
    char           m_filename[MAX_FILENAME_LEN+4];
    int            m_mVolume,m_mSpeed,res1,res2;
};

struct instData
{
    struct configData config;
    mpWindow         *m_mpWindow;
    int               m_callbackID;
    unsigned char     m_digi1;
    bool              m_running;
    char              m_newFilename[MAX_FILENAME_LEN+4];
    EventHandler     *m_eventHandler;
};

#endif //LIBIO_HMI_MEDIAPLAYER_H
