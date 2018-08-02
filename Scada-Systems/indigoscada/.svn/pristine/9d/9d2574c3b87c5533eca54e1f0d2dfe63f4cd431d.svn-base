#define  UNICODE
#ifdef __MINGW32__
#define _UNICODE /* for MINGW32 */
#endif
#include <windows.h>
#include <wctype.h>
#include <tchar.h>

/*************************************************************************
 OPC Common Definitions & Interfaces

  Specification Type:	Industry Standard Specification		
			
  Title:	        OPC Common Definitions	

  Date:	                October 27, 1998
			
  Version:	        1.0	Soft	MS-Word

  Source:	        OpcComn.doc
			
  Author:	        Opc Task Force	
  
  Status:	        Release
			
 **************************************************************************
 9. Appendix B - Sample String Filter Function
        This function provides essentially the same functionality 
        as the LIKE operator in Visual Basic.
 *************************************************************************/

inline int ConvertCase( int c, BOOL bCaseSensitive )
{
	return bCaseSensitive ? c : towupper(c);
}

//*************************************************************************          
// return TRUE if String Matches Pattern -- 
// -- uses Visual Basic LIKE operator syntax
// CAUTION: Function is recursive
//*************************************************************************
BOOL MatchPattern( LPCTSTR String, LPCTSTR Pattern, BOOL bCaseSensitive )
{ 
    TCHAR   c, p, l;
    for (; ;)
    {
        switch (p = ConvertCase( *Pattern++, bCaseSensitive ) )
        {
        case 0:                             // end of pattern
            return *String ? FALSE : TRUE;  // if end of string TRUE

        case _T('*'):
            while (*String) 
            {               // match zero or more char
                if (MatchPattern (String++, Pattern, bCaseSensitive))
                    return TRUE; 
            }
            return MatchPattern (String, Pattern, bCaseSensitive );

        case _T('?'):
            if (*String++ == 0)             // match any one char 
                return FALSE;               // not end of string 
            break; 

        case _T('['): 
            // match char set 
            if ( (c = ConvertCase( *String++, bCaseSensitive) ) == 0)
                return FALSE;                // syntax 
            l = 0; 
            if( *Pattern == _T('!') )  // match a char if NOT in set []
            {
                ++Pattern;

                while( (p = ConvertCase( *Pattern++, bCaseSensitive) )
                         != _T('\0') ) 
                {
                    if (p == _T(']'))     // if end of char set, then 
                        break;            // no match found 

                    if (p == _T('-')) 
                    {   // check a range of chars? 
                        p = ConvertCase( *Pattern, bCaseSensitive );
                        // get high limit of range 
                        if (p == 0  ||  p == _T(']')) 
                            return FALSE;     // syntax 

                        if (c >= l  &&  c <= p) 
                            return FALSE;     // if in range, return FALSE 
                    } 
                    l = p;
                    if (c == p)               // if char matches this element 
                        return FALSE;         // return false 
                } 
            }
            else    // match if char is in set []
            {
                while( (p = ConvertCase( *Pattern++, bCaseSensitive) ) 
                         != _T('\0') ) 
                {
                    if (p == _T(']'))         // if end of char set, then 
                        return FALSE;         // no match found 

                    if (p == _T('-')) 
                    {   // check a range of chars? 
                        p = ConvertCase( *Pattern, bCaseSensitive );
                        // get high limit of range 
                        if (p == 0  ||  p == _T(']')) 
                            return FALSE;       // syntax 

                        if (c >= l  &&  c <= p) 
                            break;              // if in range, move on 
                    } 
                    l = p;
                    if (c == p)                 // if char matches this element 
                        break;                  // move on 
                } 

                while (p  &&  p != _T(']'))     // got a match in char set 
                    p = *Pattern++;             // skip to end of set 
            }

            break; 

        case _T('#'):
            c = *String++; 
            if( !_istdigit( c ) )
                return FALSE;        // not a digit

            break;

        default: 
            c = ConvertCase( *String++, bCaseSensitive ); 
            if( c != p )            // check for exact char 
                return FALSE;                   // not a match 

            break; 
        } 
    } 
} 

