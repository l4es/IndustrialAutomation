package com.oapc.jcommon;

import java.util.*;
import java.io.*;

import android.content.*;



public class oapcResourceBundle extends HashMap<String,String>
{
   private Context ctx;
	
   public oapcResourceBundle(Context ctx,String resName)
   {
	  super();
	  this.ctx=ctx;
      appendResource(resName);
   }


   public void appendResource(String resName)
   {
      Locale          locale;
      String          locName,pathName,ln;
      BufferedReader  in=null;
      int             cnt=1,resID;

      locale=Locale.getDefault();
      locName=locale.getISO3Country()+"_"+locale.getISO3Language();
      pathName=resName+"_"+locName;
      resID=ctx.getResources().getIdentifier(pathName,"raw",ctx.getPackageName());    
      try
      {
         in=new BufferedReader(new InputStreamReader(ctx.getResources().openRawResource(resID),"UTF-8"));
      }
      catch (Exception e)
      {
         in=null;
      }
      if (in==null)
      {
          locName=locale.getISO3Language();
          pathName=resName+"_"+locName;
          resID=ctx.getResources().getIdentifier(pathName,"raw",ctx.getPackageName());
          try
          {
             in=new BufferedReader(new InputStreamReader(ctx.getResources().openRawResource(resID),"UTF-8"));
          }
          catch (Exception e)
          {
             in=null;
          }
          if (in==null)
          {          
/*	          wxInt32 separatorPos;

	          separatorPos=resName.Find(wxFileName::GetPathSeparator(),true);
	          if (separatorPos<0) separatorPos=resName.Find('/',true);
	          if (separatorPos<0) separatorPos=resName.Find('\\',true);
	          dirName=resName.Mid(0,separatorPos+1);
	          fileName=resName.Mid(separatorPos+1);
	          wxDir dir(dirName);

	          if (dir.IsOpened())
	          {
	             if (dir.GetFirst(&pathName,fileName+_T("_")+locName+_T("_*.property")))
	             {
	                fHandle.Open(dirName+pathName); // use a locale of the same language but of an other country variant that is available
	             }
	          }*/
	       }
      }
	   if (in!=null)
	   {
	      try
         {
            do
	         {
               ln=in.readLine();
               processLine(ln,cnt);
               cnt++;
            }
            while (ln!=null);
         } 
	      catch (IOException ioe)
	      {
	      }
	      finally
	      {
	         try
	         {
	            in.close();
	         } 
	         catch (IOException ioe)
	         {
	         }	        
	      }
	   }
   }



   private void processLine(String ln,int cnt)
   {
      int    delim;
	   String key,value;

	   if (ln==null) return;
      ln=ln.trim();
	   if (ln.length()<5) return; // line much too short
	   if (ln.startsWith("#")) return; // starts with comment identifier
	   ln=ln.replace("\\n","\n");
	   delim=ln.indexOf(" = ");
      if (delim<=0)
	   {
	      System.err.println("oapcResourceBundle: Illegal format / assignment in line "+cnt);
         return;
	   }
	   key=ln.substring(0,delim);
      value=ln.substring(delim+3);
      put(key, value);
   }



   public String getString(String key)
   {
      String ret;

      if (key.length()<=0) return key;
      ret=get(key);
      if ((ret==null) || (ret.length()<=0))
      {
         ret=key;
	      key.replace("\n","\\n");
	   }
      return ret;
   }
}
