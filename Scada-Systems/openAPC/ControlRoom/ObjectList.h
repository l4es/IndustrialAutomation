#ifdef ENV_INT
#include "../ObjectList.h"
#endif

#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#ifndef ENV_HPLAYER
 #include "hmiObject.h"
 #include "BasePanel.h"
 #include "hmiMutex.h"
 #include "UserData.h"
#endif
#include "flowObject.h"
#include <list>
#include <map>

class flowGroup;

class ObjectList:public wxList
{
public:
   ObjectList(flowGroup *owner);
   ObjectList();
   virtual ~ObjectList();

   void        init();

#ifndef ENV_HPLAYER
   ObjectList(HMICanvas *parent/*,wxByte handleID*/);
   hmiObject  *getUIElement(wxInt32 wxId,wxUint32 parentID,wxPoint pos);
   hmiMutex   *getMutexByName(wxString name);
   hmiMutex   *getMutexByObjectID(wxInt32 id);
   wxRealPoint getSingleSize();
#endif

   void        setSingleSize(wxRealPoint newSize);
   wxRealPoint getTotalSize();
   wxRealPoint getTotalPos();
   void        setTotalPos(wxRealPoint newPos);
   void        deleteAll(bool deleteData);
   void        Append(ObjectList *list);
   void        Insert(flowObject* object);

   void        addObject(flowObject* object,bool display,bool keepID);
   void        deleteObject(flowObject *object,bool unlinkConnections,bool destroyObject);
   void        deleteObjects(ObjectList list,bool unlinkConnections,bool destroyObjects);
   void        deleteObjects(wxUint32 withParentID,bool destroyObject);
   void        Clear();
   void        DeleteContents(bool destroy);
   wxNode     *getDrawableObject(wxList::compatibility_iterator node);
   wxUint32    getUniqueID();
   wxUint32    currentUniqueID();

#ifdef ENV_EDITOR
   wxInt32     getCount(wxUint32 mask,wxUint32 mode);
   void        InsertBefore(wxNode *posNode,flowObject* object);
   void        InsertAfter(wxNode *posNode,flowObject* object);
   void        moveFlowPos(wxPoint newPos,wxPoint beyondPos);
   wxRect      getBorder(wxInt32 dist);
   wxPoint     getSingleMinSize();
   wxPoint     getSingleMaxSize();
   void        updateUniqueID(wxUint32 newID);
   void        saveProject(wxString file,bool isCompiled);
   void        saveFlowObjectData(wxFile *FHandle,bool isCompiled);
   void        saveDeviceObjectData(wxFile *FHandle,bool isCompiled);
   wxNode     *getDrawableFlowObject(wxList::compatibility_iterator node);
   flowObject *getDrawableFlowObject(wxPoint pos,wxByte includeFlowLines);
   wxNode     *findObjectByName(wxString name,wxNode *previousOccurence,wxByte hmiObjectOnly);
#else
#ifndef ENV_HPLAYER
   void        setUserPrivileges(UserData *userData);
#endif
#endif
#ifndef ENV_HPLAYER
   void        setParent(BasePanel *parent);
#endif
   wxString    loadProject(wxString file,bool display,bool winsizeOnly);
   wxInt32     loadFlowObjectData(wxFile *FHandle,wxInt32 chunkSize,wxInt32 IDOffset,wxUint32 *loadedGroupID,bool display,bool isCompiled,ObjectList *topLevelObjectList,wxInt32 mode,bool *hasISCOElement);
   wxNode     *getObject(wxNode *node);
   flowObject *getObject(wxUint32 id);
   flowObject *getObject(wxString name,bool includeDeviceObjects);
   flowObject *getParentISCOObject(wxString name);
   flowObject *getFirstObject();
#ifndef ENV_HPLAYER
   void        saveUserData(wxFile *FHandle);
   hmiObject  *getObjectByUIElementID(wxInt32 id);

   std::list<hmiMutex*>        m_mutexList;
   std::map<wxString,wxBitmap> m_imageList;
#endif
   wxString               name;
   struct hmiProjectData *m_projectData;

private:
#ifdef ENV_EDITOR
   void            handleObjectSize(flowObject* object,bool keepID);
#endif
#ifndef ENV_HPLAYER
   wxInt32         loadUserData(wxFile *FHandle,wxInt32 chunkSize,bool checkForRuntimeData);

   BasePanel      *parent;
#endif
   wxRealPoint     pos,size;
   wxUint32        m_lastID;
//   wxByte          handleID;
   flowGroup      *m_owner;
};

#endif
