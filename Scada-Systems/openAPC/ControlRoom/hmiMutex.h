#ifndef HMI_MUTEX_H
#define HMI_MUTEX_H

#include <list>

class hmiMutex : public wxEvtHandler
{
public:
	hmiMutex(wxString name);
	virtual ~hmiMutex();

          void     addObjectID(wxUint32 id);
          void     removeObjectID(wxUint32 id);
          bool     hasObjectID(wxUint32 id);
   static wxUint32 getNextID();
   static wxUint32 s_lastID;

   wxString            m_name;
   wxInt32             m_ID;
   std::list<wxUint32> m_idQ;
#ifdef ENV_EDITOR
   bool                m_used;
#endif

private:

};

#endif

