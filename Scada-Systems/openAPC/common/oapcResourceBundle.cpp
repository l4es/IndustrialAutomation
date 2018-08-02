/******************************************************************************************

oapcResourceBundle v0.7

This code is (c) 2009-2018 by Oxy / Virtual Worlds Productions / https://virtualworlds.de

You can use and modify this code for free wherever you want and for whatever purpose you
want as long as you keep this copyright information intact wherever this code is used.
Contributions, comments and other helpful things can be sent to virtual_worlds(at)gmx.de

Additionally I'd be very glad if you mention OXY / VWP within your credits ;-)

*******************************************************************************************/



#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/textfile.h>

#include "oapcResourceBundle.h"

oapcResourceBundle    *g_res; // global resource bundle object, has to be used to enable the _-macro

WX_DECLARE_STRING_HASH_MAP( wxString, usePropHash);

usePropHash propHash;

/**
Constructor for a resource bundle that can be used to manage translations for wxWidgets
applications. The constructor expects the name of a resource file that has to reside within
the applications default directory. The file itself has to be a UTF-8-encoded text file
that contains pairs of texts within each line separated by the characters " = ". So a
translation can look like this:
<PRE>
# some comments

English text = Translated text
</PRE>
Lines that start with a "#" are ignored as comments. Lines that contain no text and only a
CR/LF are ignored too.
A valid line contains the original text (that is used within the application) on the left
side, then the delimiter " = " is set (here the blanks are important!) and the translated
thext on the right side.<BR>
Several translations are done via the name of the used resource bundle file. So if you give
a resName "myapp" you
can provide following files:
<UL>
 <LI>myapp_lc.property - a property file for a language that is used when no more specific
     translations/variants for that language are found (e.g. myapp_de.property for all
     German language variants)
 <LI>myapp_lc_LC.property - a specific property file for a very specific language (e.g.
     myapp_de_AT.property for an austrian german)
</UL>
The whole thing is intended to be similar with the resource handling known from Java(tm), so
for some more detailed information about the general usage please refer to
https://docs.oracle.com/javase/7/docs/api/java/util/ResourceBundle.html .
Please note one big difference: the Java-solution uses keys as input that need to have an
assigned value, the key itself is never shown. Here the key itself is used when no
corresponding value can be found so that key also defines the default resource text.<BR><BR>
@param resName the base name of the resource file to be used
*/
oapcResourceBundle::oapcResourceBundle(const wxString resName,const wxString tryLocale)
{
   if (resName.Length()>0) appendResource(resName,tryLocale);
}



oapcResourceBundle::~oapcResourceBundle()
{
}


/**
This method can be used to flush all stored resources e.g. in case of a language change. It
results in an empty oapcResourceBundle object without any mapped/translated texts´.
*/
void oapcResourceBundle::flushResources()
{
   propHash.clear();
}

/**
This method adds data from an other translation file to the resource data that are alreay loaded.
It can be used in cases where e.g. external libraries are added to the application during runtime
that come with their own translation data. So this function can be called before that library is
used in order to provide the required translation.
@param[in] resName base name of the resource file that has to be loaded, for a description of the
           exact file name and the format of the files contents please refer the constructor
           description above
@param[in] tryLocale when this value is set, it is tired to load a locale with this identifier
           instead of the one which belongs to the system settings. When this fails, function
           continues with default operation and tries to evaluate system settings. When it is set
           to "xxxx" no translation is loaded and default language values of the application
           will be used
*/
void oapcResourceBundle::appendResource(const wxString resName,const wxString tryLocale)
{
    wxLocale      locale;
    wxString      locName,pathName="novalidfile",ln,dirName,fileName;
    wxTextFile    fHandle;

    if (tryLocale.Length()>0)
    {
       if (tryLocale.CmpNoCase(_T("xxxx"))==0) return;
       pathName=resName+_T("_")+tryLocale+_T(".property");
    }
    if (wxFile::Exists(pathName)) fHandle.Open(pathName); // use the exact locale file
    else
    {
       locale.Init();
       locName=locale.GetName();
       pathName=resName+_T("_")+locName+_T(".property");
       if (wxFile::Exists(pathName)) fHandle.Open(pathName); // use the exact locale file
       else
       {
          locName=locName.Mid(0,locName.Find(_T("_")));
/*#ifdef _DEBUG
       pathName=_T("../OpenAPC/")+resName+_T("_")+locName+_T(".property");
#else*/
          pathName=resName+_T("_")+locName+_T(".property");
//#endif
          if (wxFile::Exists(pathName)) fHandle.Open(pathName); // use the locale of the general language
          else
          {
             wxInt32 separatorPos;

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
             }
             if (!fHandle.IsOpened()) // use a translation file without any country/locale specific extension
             {
                pathName=resName+_T(".property");
                if (wxFile::Exists(pathName)) fHandle.Open(pathName);
             }
          }
       }
    }
    if (fHandle.IsOpened())
    {
//       resLoaded=1;
       ln=fHandle.GetFirstLine();
       processLine(ln);
       while(!fHandle.Eof())
       {
          ln=fHandle.GetNextLine();
          processLine(ln);
       }
       fHandle.Close();
    }
//    else resLoaded=0;
}



void oapcResourceBundle::processLine(wxString ln)
{
   wxUint32 delim;
   wxString key,value;

   ln=ln.Trim();
   if (ln.Length()<5) return; // line much too short
   if (ln.Find(_T("#"))==0) return; // starts with comment identifier
   ln.Replace(_T("\\n"),_T("\n"));
   delim=ln.Find(_T(" = "));
   if (delim<=0)
   {
      wxLogDebug(_T("oapcResourceBundle: Illegal format / assignment in line %s"),ln);
      return;
   }
   key=ln.Mid(0,delim);
   value=ln.Mid(delim+3);
   propHash[key]=value;
}



/**
This method tries to get a translation for the given original text. If no translation could
be found a warning is printed out (in debug versions only) and the original string is
returned so that it can be used as the default text. That means that the key has to be a
meaningful text and not an abbreviation nobody should see.
@param[in] key the text that has to be translated
@return the translated text according to the current locale settings or the value of key if
        no translation could be found for it
*/
wxString oapcResourceBundle::getString(wxString key)
{
   wxString ret;

   if (key.Length()<=0) return key;
   ret=propHash[key];
   if (ret.Length()<=0)
   {
       ret=key;
       key.Replace(_T("\n"),_T("\\n"));
#ifdef _DEBUG
       // *** Enable the following code to write these texts into a file that are not translated 
/*       {
          wxFile fHandle;

          fHandle.Open(_T("property.txt"),wxFile::write_append);
          if (fHandle.IsOpened())
          {
#ifdef ENV_LINUX
             fHandle.Write(key+_T(" = \n"));
#else
             fHandle.Write(key+_T(" = \r\n"));
#endif
             fHandle.Close();
          }
       }*/
#endif
   }
   return ret;
}


