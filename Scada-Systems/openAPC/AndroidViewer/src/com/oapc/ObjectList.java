package com.oapc;

import java.util.*;
import java.io.*;

import android.graphics.*;
import android.view.*;

import com.oapc.jcommon.*;

class ObjectList extends Vector<flowObject>
{
           hmiProjectData   m_projectData=null;
           String           name;
   private BasePanel        parent;
   private Point            pos,size;
   private int              m_lastID;
           Vector<hmiMutex> m_mutexList;
   
//   private flowGroup      m_owner;
   
   ObjectList()
   {
   }
   
   
   void init()
   {
      pos.x=-1;  pos.y=-1;
      size.x=-1; size.y=-1;
      name="";
      m_lastID=0;
   }


   ObjectList(HMICanvas parent)
   {
      super();
      init();
      this.parent=parent;
//      m_owner=null;
      m_projectData=null;
   }   

   
   
   void setParent(BasePanel parent)
   {
      this.parent=parent;
   }

   
   
   void DeleteContents(boolean destroy)
   {
      flowObject object;

      object=firstElement();
      while (object!=null)
      {
         if (destroy)
         {
            if (object.data.isChildOf!=0) object.m_doDestroy=false; // child objects are deleted by their parents
            object.close();
         }
         remove(object);
         object=firstElement();
      }
   }   
   
   
   
   void deleteAll(boolean deleteData)
   {
      if (this==MainWin.g_objectList) hmiMutex.s_lastID=0;
      if ((this==MainWin.g_objectList) && (MainWin.g_objectList.m_mutexList.size()>0))
       MainWin.g_objectList.clear();
      if (deleteData) DeleteContents(true);
      else clear();
      pos.x=-1;  pos.y=-1;
      size.x=-1; size.y=-1;
   }

   

   private static int getObject_ctr;
   
   flowObject getObject(flowObject object)
   {
	  // TODO: find thread-safe solution!

      try
	  {	
         if (object==null) getObject_ctr=0;
         else getObject_ctr++;
         return elementAt(getObject_ctr);
	  }
      catch (ArrayIndexOutOfBoundsException aioobe)
      {
      }
      return null;
   }



   flowObject getObject(int id)
   {
      flowObject object;

      object=getObject(null);
      while (object!=null)
      {
         if (object.data.id==id) return object;
         object=getObject(object);
      }
      return null;
   }

   
   
   /**
    * Finds a flow object based on its (unique) name
    * @param[in] name the unique name of the object
    * @param[in] includeDeviceObjects when this parameter is set to true also object with mode
    *            OBJECT_MODE_DEVICE are included into search, elsewhere they are ignored and not returned
    * @return the object with this name or NULL in case it could not be found
    */
   flowObject getObject(String name,boolean includeDeviceObjects)
   {
      flowObject object;

      object=getObject(null);
      while (object!=null)
      {
         if (name.contentEquals(object.data.name))
         {
            if ((includeDeviceObjects) || (object.getMode()!=flowObject.OBJECT_MODE_DEVICE))
             return object;
         }
         object=getObject(object);
      }
      return null;
   }



   /**
   * Find a hmiMutex object according to the ID of the object that is managed
   * by this mutex
   */
   hmiMutex getMutexByObjectID(int id)
   {
      int      i;
      hmiMutex mutex;
      
      for (i=0; i<MainWin.g_objectList.m_mutexList.size(); i++)
      {
         mutex=MainWin.g_objectList.m_mutexList.elementAt(i);
         if (mutex.hasObjectID(id)) return mutex;
      }
      return null;
   }
   
   
   
   int getUniqueID()
   {
      m_lastID++;
      return m_lastID;
   }



   int currentUniqueID()
   {
      return m_lastID;
   }

   
   
   void addObject(flowObject object)
   {
      this.add(object);
   }   

   
   hmiObject getObjectByViewObject(View v)
   {
      hmiObject object;

      object=(hmiObject)getObject(null);
      while (object!=null)
      {
         if ((object.uiElement!=null) && (object.uiElement==v)) return object;
/*         else if (object.data.type==hmiObject.HMI_TYPE_NUMFIELD)
         {
            if (((oapcSpinCtrl)object.uiElement).m_numField==v) return object;
            if (((oapcSpinCtrl)object.uiElement).m_spinButton==v) return object;
         }*/
         object=(hmiObject)getObject(object);
      }
      return null;
   }
   
   
   
   private int loadFlowObjectData(DataInputStream FHandle,int chunkSize,int IDOffset,Int loadedGroupID,ObjectList topLevelObjectList,int mode,Bool hasISCOElement)
   {
      int        subChunkSize;
      byte[]     rawData={0,0,0,0};
      String     chunkName;
      int        readSize;
      flowObject loadObject;

      try
      {
         while (chunkSize>0)
	     {
	        FHandle.read(rawData,0,4);
	        chunkName=new String(rawData);
	        chunkSize-=4;
	        chunkName=new String(rawData);
	        subChunkSize=FHandle.readInt();
	        chunkSize-=4;
	        assert(subChunkSize>0);
	        if (subChunkSize<=0) return chunkSize;
	
	        chunkSize-=subChunkSize; // the following code ensures that exactly the size of subChunkSize is read so we can substract it here
	        if (chunkName.compareTo(iff.CHUNK_FLOW_SOBJ)==0) // hmi objects
	        {
	           hmiFlowData flowData=new hmiFlowData();
	
	           assert(mode!=flowObject.OBJECT_MODE_DEVICE);
	           subChunkSize-=flowData.load(FHandle);
	           if (IDOffset!=0)
	            flowData.id=flowData.id-IDOffset+MainWin.g_objectList.currentUniqueID()+1;
	
	           loadObject=getObject(flowData.id);
	           assert(loadObject!=null);
	           if (loadObject!=null)
	           {
	              subChunkSize-=loadObject.loadFlow(FHandle,flowData,0,false);
	              ((hmiObject)loadObject).m_flowFlags=flowData.flowFlags;
	           }
	        }
	/*         else if (!strncmp(rawData,CHUNK_FLOW_EXIO,4)) // external IO flow objects
	         {
	            struct externalFlowData        flowData;
	            flowExternalIOLib             *flow=NULL;
	            ExternalIOLib                 *extIOLib;
	
	            subChunkSize-=FHandle->Read(&flowData,sizeof(struct externalFlowData));
	            flowData.flowData.version=  ntohl(flowData.flowData.version);
	            if (IDOffset)
	            {
	               wxASSERT(display); // add-loading is not supported for background-loaded jobs
	               flowData.flowData.id=ntohl(flowData.flowData.id)-IDOffset+g_objectList.currentUniqueID()+1;
	            }
	            else flowData.flowData.id  =ntohl(flowData.flowData.id);
	            if ((*hasISCOElement) || (mode!=OBJECT_MODE_DEVICE)) // load and create the plug in only in case there is a ISConnection
	                                                                 // available or in case we're loading locally managed objects
	            {
	               flowData.flowData.usedFlows=ntohl(flowData.flowData.usedFlows);
	               flowData.flowData.maxEdges= ntohl(flowData.flowData.maxEdges);
	               flowData.flowX=             ntohl(flowData.flowX);
	               flowData.flowY=             ntohl(flowData.flowY);
	               flowData.extLibDataSize=    ntohl(flowData.extLibDataSize);
	               flowData.userPriviFlags    =oapc_util_ntohll(flowData.userPriviFlags);
	               flowData.dataFlowFlags     =ntohl(flowData.dataFlowFlags);
	               flowData.flowFlags         =ntohl(flowData.flowFlags);
	               flowData.type              =ntohl(flowData.type);
	               flowData.cycleMicros       =ntohl(flowData.cycleMicros);
	               wxString libname=g_charToStringUTF16BE(flowData.store_libname,sizeof(flowData.store_libname));
	      #ifdef ENV_LINUX
	               libname.Replace(_T(".dll"),_T(".so"));
	               libname.Replace(_T(".DLL"),_T(".so"));
	               libname.Replace(_T("\\"),_T("/"));
	      #endif
	      #ifdef ENV_WINDOWS
	               libname.Replace(_T(".so"),_T(".dll"));
	               libname.Replace(_T("/"),_T("\\"));
	      #endif
	
	               if (!g_externalIOLibs->contains(libname))
	                g_externalIOLibs->importPlugin(libname,FLOW_TYPE_MASK,0);
	               extIOLib=g_externalIOLibs->getLib(true);
	               while (extIOLib)
	               {
	                  if (extIOLib->libname.Cmp(libname)==0)
	                  {
	                     flow=new flowExternalIOLib(extIOLib,flowData.flowData.id);
	                     if (flow)
	                     {
	                        if (flow->returnOK)
	                        {
	                           flow->name=g_charToStringUTF16BE(flowData.store_name,sizeof(flowData.store_name));
	      #ifndef ENV_EDITOR
	                           flow->createNodeNames();
	      #endif
	                           addObject(flow,display,IDOffset>0);
	                           flow->setMode(mode);
	      #ifdef ENV_EDITOR
	                           if (mode==OBJECT_MODE_FLOWELEMENT)
	                            flow->setFlowPos(this,wxPoint(flowData.flowX,flowData.flowY),1,1);
	                           else
	                            g_plugInPanel->addItem(flow,extIOLib->getLibCapabilities());
	      #endif
	                        }
	                        else delete flow;
	                     }
	                     break;
	                  }
	                  extIOLib=g_externalIOLibs->getLib(false);
	               }
	
	               if (flow)
	               {
	                  subChunkSize-=flow->loadEXIO(FHandle,&flowData,IDOffset,isCompiled);
	                  if (!flow->returnOK)
	                  {
	                     this->DeleteObject(flow);
	                     delete flow;
	                  }
	               }
	               else
	   #ifndef ENV_HPLAYER
	                wxMessageBox(_("Could not find required Plug-In")+_T(": ")+libname,_("Error"),wxICON_ERROR|wxOK);
	   #else
	                printf("Error: Could not find required Plug-In!\n");
	   #endif
	            }
	   #ifdef ENV_PLAYER
	            if ((mode==OBJECT_MODE_DEVICE) &&
	                (((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_SINGLE_LOCAL) ||
	                 ((g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)==PROJECT_FLAG_ISPACE_DOUBLE_LOCAL)))
	            {
	               // store identifiers for automatic execution of OpenPlugger with these elements
	               g_pluggerIDList.push_back(flowData.flowData.id);
	            }
	   #endif
	         }
	         else if (!strncmp(rawData,CHUNK_FLOW_FGRP,4)) // flowgroup
	         {
	            wxASSERT(!isCompiled);
	            wxASSERT(mode!=OBJECT_MODE_DEVICE);
	            flowGroup *group=new flowGroup(m_owner,display);
	            wxASSERT(group);
	            if (group)
	            {
	               subChunkSize-=group->loadFGRP(FHandle,subChunkSize,IDOffset,display,topLevelObjectList);
	   #ifdef ENV_EDITOR
	               if (display) group->createFlowBitmap();
	   #endif
	               wxASSERT(group->returnOK);
	               if (!group->returnOK)
	               {
	                  delete group;
	                  group=NULL;
	                  if (loadedGroupID) *loadedGroupID=0;
	               }
	               else
	               {
	                  addObject(group,display,IDOffset>0);
	                  if (loadedGroupID) *loadedGroupID=group->data.id;
	               }
	            }
	         }
	         else if ((!strncmp(rawData,CHUNK_FLOW_CVDN,4)) || (!strncmp(rawData,CHUNK_FLOW_CVDC,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_CVCN,4)) || (!strncmp(rawData,CHUNK_FLOW_CVCD,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_CVND,4)) || (!strncmp(rawData,CHUNK_FLOW_CVNC,4)) || (!strncmp(rawData,CHUNK_FLOW_CVNB,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_CVMC,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_CVDP,4)) || (!strncmp(rawData,CHUNK_FLOW_CVNP,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_CVCP,4)) || (!strncmp(rawData,CHUNK_FLOW_CVBP,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_CVPC,4)) || (!strncmp(rawData,CHUNK_FLOW_CVPD,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_CVPN,4)) || (!strncmp(rawData,CHUNK_FLOW_CVPB,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_LRSF,4)) || (!strncmp(rawData,CHUNK_FLOW_LTFF,4)) || 
	                  (!strncmp(rawData,CHUNK_FLOW_LGDN,4)) || (!strncmp(rawData,CHUNK_FLOW_LGNN,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_LGDP,4)) || (!strncmp(rawData,CHUNK_FLOW_LGNP,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_LDSF,4)) || (!strncmp(rawData,CHUNK_FLOW_LNSF,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_LCSF,4)) || (!strncmp(rawData,CHUNK_FLOW_LBSF,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_LGCP,4)) || (!strncmp(rawData,CHUNK_FLOW_LGBP,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_LGDO,4)) || (!strncmp(rawData,CHUNK_FLOW_LGNO,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_LGDA,4)) || (!strncmp(rawData,CHUNK_FLOW_LGNA,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_LGDX,4)) || (!strncmp(rawData,CHUNK_FLOW_LGNX,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_DCMP,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_NCMP,4)) || (!strncmp(rawData,CHUNK_FLOW_CCMP,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_DCTR,4)) || (!strncmp(rawData,CHUNK_FLOW_NCTR,4)) || 
	                  (!strncmp(rawData,CHUNK_FLOW_CCTR,4)) || (!strncmp(rawData,CHUNK_FLOW_BCTR,4)) || 
	                  (!strncmp(rawData,CHUNK_FLOW_FSTP,4)) || (!strncmp(rawData,CHUNK_FLOW_FDLY,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_FSTA,4)) || (!strncmp(rawData,CHUNK_FLOW_FTMR,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_MAAD,4)) || (!strncmp(rawData,CHUNK_FLOW_MAMU,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_MASU,4)) || (!strncmp(rawData,CHUNK_FLOW_MADI,4)) ||
	                  (!strncmp(rawData,CHUNK_MISC_LOGO,4)) || (!strncmp(rawData,CHUNK_MISC_LOGR,4)) ||
	                  (!strncmp(rawData,CHUNK_MISC_USLI,4)) || (!strncmp(rawData,CHUNK_MISC_ISCO,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_NTGT,4)) || (!strncmp(rawData,CHUNK_FLOW_CTGT,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_BTGT,4)) || (!strncmp(rawData,CHUNK_FLOW_DTGT,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_NGTE,4)) || (!strncmp(rawData,CHUNK_FLOW_CGTE,4)) ||
	                  (!strncmp(rawData,CHUNK_FLOW_BGTE,4)) || (!strncmp(rawData,CHUNK_FLOW_DGTE,4)))
	         {
	            flowObject *loadObject=NULL;
	
	            wxASSERT(mode!=OBJECT_MODE_DEVICE);
	            if (!strncmp(rawData,CHUNK_FLOW_CVDN,4))
	             loadObject=new flowConverterDigi2Num();
	            else if (!strncmp(rawData,CHUNK_FLOW_CVDC,4))
	             loadObject=new flowConverterDigi2Char();
	            else if (!strncmp(rawData,CHUNK_FLOW_CVCN,4))
	             loadObject=new flowConverterChar2Num();
	            else if (!strncmp(rawData,CHUNK_FLOW_CVCD,4))
	             loadObject=new flowConverterChar2Digi();
	            else if (!strncmp(rawData,CHUNK_FLOW_CVND,4))
	             loadObject=new flowConverterNum2Digi();
	            else if (!strncmp(rawData,CHUNK_FLOW_CVNB,4))
	             loadObject=new flowConverterNum2Bits();
	            else if (!strncmp(rawData,CHUNK_FLOW_CVNC,4))
	             loadObject=new flowConverterNum2Char();
	            else if (!strncmp(rawData,CHUNK_FLOW_CVMC,4))
	             loadObject=new flowConverterMixed2Char();
	            else if (!strncmp(rawData,CHUNK_FLOW_CVDP,4))
	             loadObject=new flowConverterDigi2Pair();
	            else if (!strncmp(rawData,CHUNK_FLOW_CVNP,4))
	             loadObject=new flowConverterNum2Pair();
	            else if (!strncmp(rawData,CHUNK_FLOW_CVCP,4))
	             loadObject=new flowConverterChar2Pair();
	            else if (!strncmp(rawData,CHUNK_FLOW_CVPC,4))
	             loadObject=new flowConverterPair2Char();
	            else if (!strncmp(rawData,CHUNK_FLOW_CVPD,4))
	             loadObject=new flowConverterPair2Digi();
	            else if (!strncmp(rawData,CHUNK_FLOW_CVPN,4))
	             loadObject=new flowConverterPair2Num();
	            else if (!strncmp(rawData,CHUNK_FLOW_CVPB,4))
	             loadObject=new flowConverterPair2Bin();
	            else if (!strncmp(rawData,CHUNK_FLOW_CVBP,4))
	             loadObject=new flowConverterBin2Pair();
	
	            else if (!strncmp(rawData,CHUNK_FLOW_LRSF,4))
	             loadObject=new flowLogicDigiRSFF();
	            else if (!strncmp(rawData,CHUNK_FLOW_LTFF,4))
	             loadObject=new flowLogicDigiTFF();
	            else if (!strncmp(rawData,CHUNK_FLOW_LGDN,4))
	             loadObject=new flowLogicDigiNOT();
	            else if (!strncmp(rawData,CHUNK_FLOW_LGNN,4))
	             loadObject=new flowLogicNumNOT();
	            else if (!strncmp(rawData,CHUNK_FLOW_LGDP,4))
	             loadObject=new flowLogicDigiNOP();
	            else if (!strncmp(rawData,CHUNK_FLOW_LGNP,4))
	             loadObject=new flowLogicNumNOP();
	            else if (!strncmp(rawData,CHUNK_FLOW_LGCP,4))
	             loadObject=new flowLogicCharNOP();
	            else if (!strncmp(rawData,CHUNK_FLOW_LGBP,4))
	             loadObject=new flowLogicBinNOP();
	            else if (!strncmp(rawData,CHUNK_FLOW_NTGT,4))
	             loadObject=new flowNumTrigGate();
	            else if (!strncmp(rawData,CHUNK_FLOW_NGTE,4))
	             loadObject=new flowNumGate();
	            else if (!strncmp(rawData,CHUNK_FLOW_CTGT,4))
	             loadObject=new flowCharTrigGate();
	            else if (!strncmp(rawData,CHUNK_FLOW_CGTE,4))
	             loadObject=new flowCharGate();
	            else if (!strncmp(rawData,CHUNK_FLOW_BTGT,4))
	             loadObject=new flowBinTrigGate();
	            else if (!strncmp(rawData,CHUNK_FLOW_BGTE,4))
	             loadObject=new flowBinGate();
	            else if (!strncmp(rawData,CHUNK_FLOW_DTGT,4))
	             loadObject=new flowDigiTrigGate();
	            else if (!strncmp(rawData,CHUNK_FLOW_DGTE,4))
	             loadObject=new flowDigiGate();
	            else if (!strncmp(rawData,CHUNK_FLOW_LDSF,4))
	             loadObject=new flowLogicDigiShift();
	            else if (!strncmp(rawData,CHUNK_FLOW_LNSF,4))
	             loadObject=new flowLogicNumShift();
	            else if (!strncmp(rawData,CHUNK_FLOW_LCSF,4))
	             loadObject=new flowLogicCharShift();
	            else if (!strncmp(rawData,CHUNK_FLOW_LBSF,4))
	             loadObject=new flowLogicBinShift();
	
	            else if (!strncmp(rawData,CHUNK_FLOW_DCMP,4))
	             loadObject=new flowMathDigiCmp();
	            else if (!strncmp(rawData,CHUNK_FLOW_NCMP,4))
	             loadObject=new flowMathNumCmp();
	            else if (!strncmp(rawData,CHUNK_FLOW_CCMP,4))
	             loadObject=new flowMathCharCmp();
	            else if (!strncmp(rawData,CHUNK_FLOW_DCTR,4))
	             loadObject=new flowMathDigiCtr();
	            else if (!strncmp(rawData,CHUNK_FLOW_NCTR,4))
	             loadObject=new flowMathNumCtr();
	            else if (!strncmp(rawData,CHUNK_FLOW_CCTR,4))
	             loadObject=new flowMathCharCtr();
	            else if (!strncmp(rawData,CHUNK_FLOW_BCTR,4))
	             loadObject=new flowMathBinCtr();
	
	            else if (!strncmp(rawData,CHUNK_FLOW_LGDO,4))
	             loadObject=new flowLogicDigiConcat(hmiObject::FLOW_TYPE_LOGI_DIGIOR);
	            else if (!strncmp(rawData,CHUNK_FLOW_LGNO,4))
	             loadObject=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMOR);
	            else if (!strncmp(rawData,CHUNK_FLOW_LGDX,4))
	             loadObject=new flowLogicDigiConcat(hmiObject::FLOW_TYPE_LOGI_DIGIXOR);
	            else if (!strncmp(rawData,CHUNK_FLOW_LGNX,4))
	             loadObject=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMXOR);
	            else if (!strncmp(rawData,CHUNK_FLOW_LGDA,4))
	             loadObject=new flowLogicDigiConcat(hmiObject::FLOW_TYPE_LOGI_DIGIAND);
	            else if (!strncmp(rawData,CHUNK_FLOW_LGNA,4))
	             loadObject=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMAND);
	
	            else if (!strncmp(rawData,CHUNK_FLOW_MAAD,4))
	             loadObject=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMADD);
	            else if (!strncmp(rawData,CHUNK_FLOW_MASU,4))
	             loadObject=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMSUB);
	            else if (!strncmp(rawData,CHUNK_FLOW_MAMU,4))
	             loadObject=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMMUL);
	            else if (!strncmp(rawData,CHUNK_FLOW_MADI,4))
	             loadObject=new flowLogicNumConcat(hmiObject::FLOW_TYPE_LOGI_NUMDIV);
	
	            else if (!strncmp(rawData,CHUNK_FLOW_FSTP,4))
	             loadObject=new flowStop();
	            else if (!strncmp(rawData,CHUNK_FLOW_FSTA,4))
	             loadObject=new flowStart();
	            else if (!strncmp(rawData,CHUNK_FLOW_FDLY,4))
	             loadObject=new flowDelay();
	            else if (!strncmp(rawData,CHUNK_FLOW_FTMR,4))
	             loadObject=new flowTimer();
	
	            else if (!strncmp(rawData,CHUNK_MISC_LOGO,4))
	            {
	               if (g_flowLog==NULL)
	               {
	                  loadObject=new flowLog();
	                  g_flowLog=(flowLog*)loadObject;
	               }
	            }
	            else if (!strncmp(rawData,CHUNK_MISC_LOGR,4))
	             loadObject=new flowLogRecorder();
	            else if (!strncmp(rawData,CHUNK_MISC_USLI,4))
	             loadObject=new flowLogInUser();
	            else if (!strncmp(rawData,CHUNK_MISC_ISCO,4))
	            {
	               loadObject=new flowISConnect();
	               *hasISCOElement=true;
	            }
	
	            else wxASSERT(0);
	            if (loadObject)
	            {
	               subChunkSize-=loadObject->loadDATA(FHandle,subChunkSize,IDOffset,isCompiled);
	               addObject(loadObject,display,IDOffset>0);
	               FHandle->Seek(subChunkSize,wxFromCurrent);
	            }
	            wxASSERT(subChunkSize==0);
	         }*/
	
	         FHandle.skip(subChunkSize);
	     }
      }
      catch (IOException ioe)
      {    	  
      }
      return chunkSize;
   }



   
   
   
   String loadProject(String file)
   {
      DataInputStream in;
      String          data;
      byte[]          rawData={0,0,0,0};
      String          chunkName;
      int             chunkSize,subChunkSize=-1;
      hmiObject       loadObject;
      int             readSize;
      Bool            hasISCOElement=new Bool(false);

      if (this==MainWin.g_objectList)
      {
         hmiMutex.s_lastID=0;
         m_mutexList=null;
      }
//TODO:      g_flowLog=NULL;
      try
      {
//         in=new DataInputStream(new FileInputStream(file));
in=new DataInputStream(parent.getContext().getResources().openRawResource(R.raw.test));    	  
         in.read(rawData,0,4);
         chunkName=new String(rawData);
         if (chunkName.compareTo(iff.CHUNK_FORM)!=0)
         {
            return MainWin.g_res.getString("No valid IFF file!");
         }
         in.skip(4); // TODO: evaluate length information
         in.read(rawData,0,4);
         chunkName=new String(rawData);

         if (chunkName.compareTo(iff.CHUNK_APCX)!=0)
         {
            return MainWin.g_res.getString("No valid APCX compiled project file!");
         }
         while (in.available()>8)
         {
            in.read(rawData,0,4);
            chunkName=new String(rawData);
            chunkSize=IO.readInt(in);
            assert(chunkSize>0);
            if (chunkSize>0)
            {
               if (chunkName.compareTo(iff.CHUNK_NAME)==0)
               {
               }
               else if (chunkName.compareTo(iff.CHUNK_ANNO)==0)
               {
               }
               else if (chunkName.compareTo(iff.CHUNK_VERS)==0)
               {
               }
               else if (chunkName.compareTo(iff.CHUNK_PROJ)==0)
               {
                  m_projectData=null;
                  m_projectData=new hmiProjectData(false);
                  m_projectData.version        =IO.readInt(in); chunkSize-=4;
                  in.skip(4);                                   chunkSize-=4; // reserved1
                  m_projectData.totalW         =IO.readInt(in); chunkSize-=4; 
                  m_projectData.totalH         =IO.readInt(in); chunkSize-=4; 
                  in.skip(8);                                   chunkSize-=8; // gridw,gridH
                  m_projectData.bgCol          =wxConvert.fromWxColour(IO.readInt(in));
                                                                chunkSize-=4;                   
                  m_projectData.flags          =IO.readInt(in); chunkSize-=4;                   
                  in.skip(8);                                   chunkSize-=8; // flowW,flowH 
                  m_projectData.flowTimeout    =IO.readInt(in); chunkSize-=4;                  
                  m_projectData.timerResolution=IO.readInt(in); chunkSize-=4;
                  in.skip(16);                                  chunkSize-=16; // editorX,editorY,editorW,editorH
                   
                  if (m_projectData.version>=2)
                  {
                     // new since 1.1
                     m_projectData.touchFactor    =IO.readInt(in); chunkSize-=4;
                     if (m_projectData.touchFactor<1000) m_projectData.touchFactor=1000;
                     else if (m_projectData.touchFactor>20000) m_projectData.touchFactor=20000;

                     m_projectData.font.pointSize=IO.readInt(in);  chunkSize-=4;
                     m_projectData.font.style    =IO.readInt(in);  chunkSize-=4;
                     m_projectData.font.weight   =IO.readInt(in);  chunkSize-=4;
                     in.skip(4);                                   chunkSize-=4; //reserved1
                     in.skip(common.MAX_NAME_LENGTH*2);            chunkSize-=common.MAX_NAME_LENGTH*2; //m_projectData.font.faceName=IO.readUTF16BE(in,common.MAX_NAME_LENGTH*2);
                  }
                  if (m_projectData.version>=3)
                  {
                     // new since 1.3
                     byte[] d=new byte[19];
                       
                     in.read(d);                                   chunkSize-=19;
                     m_projectData.m_remSingleIP=new String(d);
                     m_projectData.m_remSingleIP=m_projectData.m_remSingleIP.trim();
                  }
                  if (m_projectData.version>=4)
                  {
                     // new since 1.4
                     byte[] d=new byte[19];
                        
                     in.read(d);                                   chunkSize-=19;
                     m_projectData.m_remDoubleIP=new String(d);
                     m_projectData.m_remDoubleIP=m_projectData.m_remDoubleIP.trim();
                  }
               }
/* TODO:                 else if (!strncmp(rawData,CHUNK_USPR,4))
                 {
   #ifndef ENV_HPLAYER
                    wxInt32         priviNum,l,i;
                    wxMBConvUTF16BE conv;
                    wchar_t         buf[154];
                    char            store_name[304];

                    for (i=0; i<MAX_PRIVI_NUM; i++) g_userPriviData.priviName[i]=_T("");
                    FHandle.Read(&l,4); chunkSize-=4;
                    g_userPriviData.enabled=ntohl(l)!=0;
                    if (g_userPriviData.enabled) g_userList=new UserList();

                    FHandle.Read(&l,4); chunkSize-=4;
                    g_userPriviData.privisEnabled=ntohl(l);

                    if (chunkSize>0)
                    {
                       // available only in non-compiled files
                       FHandle.Read(&l,4); chunkSize-=4;
                       priviNum=ntohl(l);
                       if (priviNum>MAX_PRIVI_NUM) priviNum=MAX_PRIVI_NUM;
                       for (i=0; i<priviNum; i++)
                       {
                          if (chunkSize<300) break;
                          FHandle.Read(store_name,300);     
                          conv.MB2WC(buf,store_name,sizeof(buf));
                          g_userPriviData.priviName[i]=buf;
                          chunkSize-=300;
                       }
                       // end of available only in non-compiled files
                    }
   #endif
                 }
                 else if (!strncmp(rawData,CHUNK_USDA,4))
                 {
   #ifndef ENV_HPLAYER
                    chunkSize=loadUserData(FHandle,chunkSize,true);
   #endif
                 }*/
               else if (chunkName.compareTo(iff.CHUNK_HOBL)==0)
               {
                  while (chunkSize>0)
                  {
                      in.read(rawData);
                      chunkName=new String(rawData);
                      subChunkSize=IO.readInt(in);
                      chunkSize-=8;
                      chunkSize-=subChunkSize; // the following code ensures that exactly the size of subChunjkSize is read so we can substract it here
                      loadObject=null;
                      if (chunkName.compareTo(iff.CHUNK_HOBL_MTXL)==0)
                      {
                          if (subChunkSize>=200)
                          {
                             hmiMutex mutex;
                             int      id;

                             if (m_mutexList==null) m_mutexList=new Vector<hmiMutex>();
                             in.skip(200);
                             mutex=new hmiMutex();
                             subChunkSize-=200;
                             m_mutexList.add(mutex);
                             while (subChunkSize>=4)
                             {
                                id=IO.readInt(in);     
                                mutex.addObjectID(id);
                                subChunkSize-=4;
                             }
                          }
                      }
                      else if (chunkName.compareTo(iff.CHUNK_HOBL_SIBU)==0)
                       loadObject=new hmiSimpleButton(parent,null);
/*                       else if (!strncmp(rawData,CHUNK_HOBL_FLTF,4))      
                        loadObject=new hmiFloatField(parent,NULL);
                       else if (!strncmp(rawData,CHUNK_HOBL_HGAG,4)) loadObject=new hmiHGauge(parent,NULL);*/
                      else if (chunkName.compareTo(iff.CHUNK_HOBL_HSLD)==0)
                       loadObject=new hmiHSlider(parent,null);
/*                       else if (!strncmp(rawData,CHUNK_HOBL_IMAG,4)) loadObject=new hmiImage(parent,NULL);
                       else if (!strncmp(rawData,CHUNK_HOBL_IMBU,4)) loadObject=new hmiImageButton(parent,NULL);
                       else if (!strncmp(rawData,CHUNK_HOBL_TOIB,4)) loadObject=new hmiToggleImageButton(parent,NULL);
                       else if (!strncmp(rawData,CHUNK_HOBL_LINE,4)) loadObject=new hmiLine(parent,NULL);*/
                       else if (chunkName.compareTo(iff.CHUNK_HOBL_NUMF)==0)
                        loadObject=new hmiNumField(parent,null);
/*                       else if (!strncmp(rawData,CHUNK_HOBL_TELA,4))
                        loadObject=new hmiTextLabel(parent,NULL);
                       else if (!strncmp(rawData,CHUNK_HOBL_TAPA,4)) loadObject=new hmiTabbedPane(parent,NULL);
                       else if (!strncmp(rawData,CHUNK_HOBL_STPA,4)) loadObject=new hmiStackedPane(parent,NULL);
                       else if (!strncmp(rawData,CHUNK_HOBL_ADPA,4)) loadObject=new hmiAdditionalPane(parent,NULL);
                       else if (!strncmp(rawData,CHUNK_HOBL_SIPA,4))
                        loadObject=new hmiSinglePanel(parent,NULL);*/
                       else if (chunkName.compareTo(iff.CHUNK_HOBL_TOBU)==0)
                        loadObject=new hmiToggleButton(parent,null);
                       else if (chunkName.compareTo(iff.CHUNK_HOBL_RABU)==0)
                        loadObject=new hmiRadioButton(parent,null,hmiObject.HMI_TYPE_RADIOBUTTON);
                       else if (chunkName.compareTo(iff.CHUNK_HOBL_CKBX)==0)
                        loadObject=new hmiRadioButton(parent,null,hmiObject.HMI_TYPE_CHECKBOX);
/*                       else if (!strncmp(rawData,CHUNK_HOBL_TXTF,4))
                        loadObject=new hmiTextField(parent,NULL,HMI_TYPE_TEXTFIELD);
                       else if (!strncmp(rawData,CHUNK_HOBL_PWDF,4))
                        loadObject=new hmiTextField(parent,NULL,HMI_TYPE_PASSWORDFIELD);
                       else if (!strncmp(rawData,CHUNK_HOBL_VGAG,4)) loadObject=new hmiVGauge(parent,NULL);*/
                       else if (chunkName.compareTo(iff.CHUNK_HOBL_VSLD)==0)
                        loadObject=new hmiVSlider(parent,null);
/*                       else if (!strncmp(rawData,CHUNK_HOBL_ANME,4))
                        loadObject=new hmiAngularMeter(parent,NULL);
                       else if (!strncmp(rawData,CHUNK_HOBL_ANRE,4)) loadObject=new hmiAngularRegulator(parent,NULL);
                       else if (!strncmp(rawData,CHUNK_HOBL_LCDN,4)) loadObject=new hmiLCDNumber(parent,NULL);
                       else if (!strncmp(rawData,CHUNK_HOBL_UMGM,4))
                        loadObject=new hmiSpecialPanel(parent,NULL,HMI_TYPE_SP_USERMANAGEMENT);
                       else if (!strncmp(rawData,CHUNK_HOBL_EXHM,4))
                       {
                          loadObject=new hmiExternalIOLib(parent,NULL);
                          if (!((hmiExternalIOLib*)loadObject).returnOK)
                          {
                             delete loadObject;
                             loadObject=NULL;
                          }
                       }
                       else wxASSERT(0);*/
                       if (loadObject!=null)
                       {
                          subChunkSize-=loadObject.load(in,subChunkSize,0);
                          if (!loadObject.returnOK)
                          {
                             loadObject.close();
                             loadObject=null;
                          }
                          if (loadObject!=null)
                          {
                             hmiMutex mutex;

                             // store the mutex info within the object that is identified by the mutex list loaded above
                             mutex=getMutexByObjectID(loadObject.data.id);
                             if (mutex!=null) loadObject.m_mutexID=mutex.m_ID;
                             // end of store the mutex info within the object that is identified by the mutex list loaded above
                             if (loadObject.data.isChildOf!=0)
                             {
                                hmiObject object;

                                object=(hmiObject)getObject(loadObject.data.isChildOf);
                                if (object!=null)
                                {
/*                                   if ((object.data.type==hmiObject.HMI_TYPE_TABBEDPANE) ||
                                       (object.data.type==hmiObject.HMI_TYPE_STACKEDPANE) ||
                                       (object.data.type==hmiObject.HMI_TYPE_ADDITIONALPANE))
                                    loadObject.setParent(((hmiTabbedPane)object).getPanel());
                                   else if (object.data.type==hmiObject.HMI_TYPE_SINGLEPANEL)
                                    loadObject.setParent((BasePanel)((hmiObject)object).uiElement);*/
                                }
                             }
                          }             
                       loadObject.createUIElement();
                       loadObject.applyData(true);
                       addObject(loadObject);
                       }
                    in.skip(subChunkSize);
                    }
//                     assert(subChunkSize==0);
                 }
                 else if (chunkName.compareTo(iff.CHUNK_FLOW)==0)
                 chunkSize=loadFlowObjectData(in,chunkSize,0,null,this,flowObject.OBJECT_MODE_FLOWELEMENT,hasISCOElement);
/*                 else if (!strncmp(rawData,CHUNK_DEVL,4))
                  {
   #ifdef ENV_EDITOR
                     hasISCOElement=true;
   #endif
                     chunkSize=loadFlowObjectData(FHandle,chunkSize,0,NULL,display,isCompiled,this,OBJECT_MODE_DEVICE,&hasISCOElement);
                  }
                  else wxASSERT(0);*/
               }
               in.skip(chunkSize);
            }
         in.close();
         }
         catch (IOException ioe)
         {
            return MainWin.g_res.getString("Could not open file!");
         }
      return "";
   }   

}
