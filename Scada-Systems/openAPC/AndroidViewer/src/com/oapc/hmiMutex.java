package com.oapc;

import java.util.*;
import com.oapc.jcommon.*;


public class hmiMutex
{
   static int         s_lastID=0;
          int         m_ID;
          Vector<Int> m_idQ=new Vector<Int>();


   hmiMutex()
   {
      m_ID=getNextID();
   }



   void addObjectID(int id)
   {
      m_idQ.add(new Int(id));
   }



   boolean hasObjectID(int id)
   {
      int i;

      for (i=0; i<m_idQ.size(); i++)
       if (m_idQ.elementAt(i).val==id) return true;
      return false;
   }



   int getNextID()
   {
      s_lastID++;
      return s_lastID;
   }



   
}
