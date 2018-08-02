/********************************************************************************************

This program and source file is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

*********************************************************************************************/

#include <wx/strconv.h>
#include <wx/string.h>

static wxString s_charToString(const char *c,wxInt32 len,wxMBConv *conv)
{
   wchar_t        *wc;
   wxString        ret;

   wc=(wchar_t*)malloc(sizeof(wchar_t)*(len+2));
   wxASSERT(wc);
   if (!wc) return wxEmptyString;
//   conv->ToWChar(wc,len,c,len);
   conv->MB2WC(wc,c,len+2);
   ret=wxString(wc);
   free(wc);
   return ret;
}


/**
 * Converts a UTF char array to a wxString
 * @param[in] c char array to be converted
 * @param[in] len maximum length of c
 * @param[out] result the wxString converted out of the chars; this value may be wxEmptyString
 *             in case the input string was empty or in case memory allocation failed
 */
OAPC_EXT_API void oapc_unicode_charToStringUTF16BE(const char *c,wxInt32 len,wxString *result)
{
   wxMBConvUTF16BE *conv=new wxMBConvUTF16BE();

   if (conv)
   {
      *result=s_charToString(c,len,conv);
      delete conv;
   }
   else *result=wxEmptyString;
}


/**
 * Converts a UTF char array to a wxString
 * @param[in] c char array to be converted
 * @param[in] len maximum length of c
 * @param[out] result the wxString converted out of the chars; this value may be wxEmptyString
 *             in case the input string was empty or in case memory allocation failed
 */
OAPC_EXT_API void oapc_unicode_charToStringUTF8(const char *c,wxInt32 len,wxString *result)
{
#if wxCHECK_VERSION(2,9,0)
   len=len;
   *result=wxString::FromUTF8(c);
#else
   wxMBConvUTF8 *conv=new wxMBConvUTF8();

   if (conv)
   {
      *result=s_charToString(c,len,conv);
      delete conv;
   }
   else *result=wxEmptyString;
#endif
}


/**
 * Converts a ASCII char array to a wxString
 * @param[in] c char array to be converted
 * @param[in] len maximum length of c
 * @param[out] result the wxString converted out of the chars; this value may be wxEmptyString
 *             in case the input string was empty or in case memory allocation failed
 */
OAPC_EXT_API void oapc_unicode_charToStringASCII(const char *c,wxInt32 len,wxString *result)
{
#if wxCHECK_VERSION(2,9,0)
   len=len;
   *result=wxString(c);
#else
   wxMBConvLibc *conv=new wxMBConvLibc();

   if (conv)
   {
      *result=s_charToString(c,len,conv);
      delete conv;
   }
   else *result=wxEmptyString;
#endif
}


/**
 * Converts a wxString to plain UTF8 char array
 * @param[in] s wxString to be converted
 * @param[out] c allocated memory area where the char data have to be stored into
 * param[in] len maximum size of the memory area that can be used by the conversion function
 */
OAPC_EXT_API void oapc_unicode_stringToCharUTF8(wxString s,char *c,wxInt32 len)
{
   wxMBConvUTF8  conv;

   if (s.Length()>0)conv.WC2MB(c,s,len);
   else c[0]=0;
}



/**
 * Converts a wxString to plain UTF8 char array
 * @param[in] s wxString to be converted
 * @param[out] c allocated memory area where the char data have to be stored into
 * param[in] len maximum size of the memory area that can be used by the conversion function
 */
OAPC_EXT_API void oapc_unicode_stringToCharUTF16BE(wxString s,char *c,wxInt32 len)
{
   wxMBConvUTF16BE  conv;

   if (s.Length()>0)conv.WC2MB(c,s,len);
   else memset(c,0,len); // multiply length with 2?
}



/**
 * Converts a wxString to a plain ASCII char array
 * @param[in] s wxString to be converted
 * @param[out] c allocated memory area where the char data have to be stored into
 * param[in] len maximum size of the memory area that can be used by the conversion function
 */
OAPC_EXT_API void oapc_unicode_stringToCharASCII(wxString s,char *c,wxInt32 len)
{
#if wxCHECK_VERSION(2,9,0)
   strncpy(c,(const char*)s.mb_str(wxConvLibc),len);
#else
   wxMBConvLibc conv;

   if (s.Length()>0)conv.WC2MB(c,s,len);
   else memset(c,0,len);
   c[s.Length()]=0;
#endif
}



/**
 * In-place conversion of a UTF16BE-encoded wide char string to a plain ASCII text; this
 * conversion is somewhat primitive, it simply assumes that only the lower 8 byte of the
 * wide char string contain relevant data that can be mapped to local ASCII directly.
 * @param[in/out] utf char pointer to the wide char data of the UTF16 string
 * @param[in] len the length (in charaters) of the string
 */
OAPC_EXT_API void oapc_unicode_utf16BEToASCII(char *utf,int len)
{
   int s,d;

   d=0;
   for (s=1; s<len/2; s+=2)
   {
      utf[d]=utf[s];
      d++;
   }
}


