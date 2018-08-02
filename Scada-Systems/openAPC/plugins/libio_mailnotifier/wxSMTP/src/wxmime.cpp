/****************************************************************************

 Project     : 
 Author      : 
 Description : 

 VERSION INFORMATION:
 File    : $Source$
 Version : $Revision$
 Date    : $Date$
 Author  : $Author$

 History: 
 $Log: wxmime.cpp,v $
 Revision 1.8  2004/05/19 08:53:12  tavasti
 Removed last defaultparameter (bug fix for previous...)

 Revision 1.7  2004/05/19 04:18:27  tavasti
 Fixes based on comments from Edwards John-BLUW23 <jedwards@motorola.com>
 - Changed m_fileName.GetName to GetFullName

 Revision 1.6  2004/05/19 04:06:26  tavasti
 Fixes based on comments from Edwards John-BLUW23 <jedwards@motorola.com>
 - Removed -m486 flags from makefile
 - Added filetypes wav & mp3
 - Removed default arguments from wxmime.cpp (only in .h)
 - Commented out iostream.h includes

 Revision 1.5  2003/11/21 12:36:46  tavasti
 - Makefilet -Wall optioilla
 - Korjattu 'j‰rkev‰t' varoitukset pois (J‰‰nyt muutama joita ei saa
   kohtuudella poistettua)

 Revision 1.4  2003/11/14 15:43:09  tavasti
 Sending email with alternatives works

 Revision 1.3  2003/11/13 17:12:15  tavasti
 - Muutettu tiedostojen nimet wx-alkuisiksi

 Revision 1.2  2003/11/07 09:17:40  tavasti
 - K‰‰ntyv‰ versio, fileheaderit lis‰tty.


****************************************************************************/

//static char cvs_id[] = "$Header: /v/CVS/olive/notifier/wxSMTP/src/wxmime.cpp,v 1.1.1.1 2004/08/27 23:12:14 paul Exp $";

/*
 * Purpose: wxWindows mime implementation
 * Author:  Frank Buﬂ
 * Created: 2002
 */

//#include <iostream.h>

#include "wxemail.h"
#include "wxbase64.h"


#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxArrayMimePart)

struct wxMimeTypeMapping
{
	const wxChar* extension;
	const wxChar* mainType;
	const wxChar* subType;
};

// extension, main type, sub type
// TODO: must be merged / integrated / substituted with wxFileType
static const wxMimeTypeMapping g_mimeTypeMappings[] =
{
	{ _T("txt"), _T("text"), _T("plain") },
	{ _T("htm"), _T("text"), _T("html") },
	{ _T("html"), _T("text"), _T("html") },
	{ _T("gif"), _T("image"), _T("gif") },
	{ _T("png"), _T("image"), _T("png") },
	{ _T("jpg"), _T("image"), _T("jpeg") },
	{ _T("jpeg"), _T("image"), _T("jpeg") },
	{ _T("pdf"), _T("application"), _T("pdf") },
	{ _T("wav"), _T("audio"), _T("wav") },
	{ _T("mp3"), _T("audio"), _T("mpeg") },
	{ _T("doc"), _T("application"), _T("msword") }
};

wxMimePart::wxMimePart(const wxFileName& fileName,
                       bool is_inline,const wxString& mainType,
                       const wxString& subType) :
    m_fileName(fileName),
    m_inline(is_inline),
    m_mainType(mainType),
    m_subType(subType)
{
    if(m_mainType.IsNull() || m_subType.IsNull()) {
	// setup default encoding
	m_mainType = _T("application");
	m_subType = _T("octet-stream");
        
	// try to determine encoding by filename extension
	wxString ext = fileName.GetExt().Lower();
	for (unsigned int i = 0; 
             i < sizeof(g_mimeTypeMappings) / sizeof(wxMimeTypeMapping); i++) {
            if (g_mimeTypeMappings[i].extension == ext) {
                m_mainType = g_mimeTypeMappings[i].mainType;
                m_subType = g_mimeTypeMappings[i].subType;
                break;
            }
	}
    } 
}

wxMimePart::wxMimePart(const wxString& data,
                       bool is_inline,const wxString& mainType,
                       const wxString& subType,const wxString& fileName) 
{    
    m_data = data;
    m_inline = is_inline;
    m_mainType = mainType;
    m_subType = subType;
    m_fileName = fileName;

    // Nothing to do?    
}


void wxMimePart::Encode(wxOutputStream& out)
{
    if(m_data.IsNull()) {   
        // TODO: error handling for every return
        // TODO: encoding it on-the-fly without loading it all in memory
        wxFileInputStream in(m_fileName.GetFullPath());
        if (!in.Ok()) return;
    
        // determine file length
        // TODO: perhaps this can be added to wxFileName for size()
        in.SeekI(0, wxFromEnd);
        size_t len = (size_t) in.TellI();
        in.SeekI(0);
        // that's possible for windows (#include <sys/types.h> 
        // and #include <sys/stat.h>):
        // struct _stat lenStat;
        // if (_stat(filename, &lenStat) == -1) return error;
        // size_t len = (size_t) lenStat.st_size;
    
        if (len == 0) return;
    
        // read file
        wxChar* pData = new wxChar[len];
        if (!pData) {
            wxASSERT_MSG(pData != NULL, _T("out of memory"));
            return;
        }
        in.Read(pData, len);
    
        // write encoded mime part to stream
        wxString cr(_T("\x00d\x00a"));
        wxString result;
        result << _T("Content-Type: ") << m_mainType << _T("/") << m_subType;
        if(! m_inline) {
            result << _T("; name=\"") << m_fileName.GetFullName() + _T("\"") << cr 
                   << _T("Content-Disposition: attachment; filename=\"") 
                   << m_fileName.GetFullName() << _T("\"");
        }
        result << cr << _T("Content-Transfer-Encoding: base64") << cr << cr
               << wxBase64::Encode(pData, len) << cr;
        //cout << ">>" << result << "<<" << endl;
        out.Write((const char*) result.GetData(), result.Length());
    } else {
        // We had data as string
        
        size_t len = m_data.Len();
        const wxChar* pData =m_data.GetData();

        // write encoded mime part to stream
        wxString cr(_T("\x00d\x00a"));
        wxString result;
        result << _T("Content-Type: ") << m_mainType << _T("/") << m_subType;
        if(! m_inline) {
            result << _T("; name=\"") << m_fileName.GetFullName() + _T("\"") << cr 
                   << _T("Content-Disposition: attachment; filename=\"") 
                   << m_fileName.GetFullName() << _T("\"");
        }
        result << cr << _T("Content-Transfer-Encoding: base64") << cr << cr
               << wxBase64::Encode(pData, len) << cr;
        //cout << ">>" << result << "<<" << endl;
        out.Write((const char*) result.GetData(), result.Length());        
    }
}

