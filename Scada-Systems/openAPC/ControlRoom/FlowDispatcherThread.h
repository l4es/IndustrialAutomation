#ifndef FlowDispatcherThread_H
#define FlowDispatcherThread_H

#ifdef ENV_PLAYER
 #include "flowWalkerThread.h"
#endif

#ifdef ENV_WINDOWSCE
#include <windows.h>
#endif

#include <wx/wx.h>
#include <wx/thread.h>

class FlowWalkerThread;

class FlowDispatcherThread : public wxThread
{
public:
   FlowDispatcherThread();
   virtual ~FlowDispatcherThread();
   virtual void         *Entry();
           void          appendNewThread(FlowWalkerThread *newThread);
#ifdef ENV_DEBUGGER
           wxInt32       getThreadNum();
#endif

protected:

private:
   std::list<FlowWalkerThread*> m_threadList,m_threadAppendList;
   void                        *m_signal,*m_listMutex;
};

#endif
