#include "BtStringTool.h"
#include <iostream>
using namespace std;

/*------- include files:
-------------------------------------------------------------------*/
const string BtStringTool::WhiteSpaces = " \t\n\r";

//*******************************************************************
// is_white_space                                             PUBLIC
//*******************************************************************
bool BtStringTool::is_white_space( const char in_char )
{
	return ( WhiteSpaces.find( in_char ) != string::npos );
}
// end of is_white_char

//*******************************************************************
// is_white_string                                            PUBLIC
//*******************************************************************
bool BtStringTool::is_white_string( const string& in_text )
{
	string::const_iterator it = in_text.begin();
	const string::const_iterator end = in_text.end();

	while( it != end ) {
		if( !is_white_space( *it ) ) return false;
		++it;
	}
	return true;
}
// end of is_white_string

//*******************************************************************
// split                                                      PUBLIC
//*******************************************************************
void BtStringTool::split( const string& in_text, list<string>& out_data, const char in_separator )
{
   string tmp = string();

   string::const_iterator it = in_text.begin();
   const string::const_iterator end = in_text.end();
   while( it != end ) {
      if( *it == in_separator ) {
         if( !tmp.empty() ) {
            string( tmp.begin(), tmp.end() ).swap( tmp );
            out_data.push_back( tmp );
            string().swap( tmp );
         }
         ++it;
         continue;
      }
      if( !is_white_space( *it ) ) tmp.push_back( *it );
      ++it;
   }
   if( !tmp.empty() ) {
      string( tmp.begin(), tmp.end() ).swap( tmp );
      out_data.push_back( tmp );
   }
}
// end of split

//*******************************************************************
// to_lower                                                   PUBLIC
//*******************************************************************
string& BtStringTool::to_lower( string& in_text ) const
{
   string::iterator it = in_text.begin();
   const string::iterator end = in_text.end();

   while( it != end ) {
      if( isupper( *it ) ) {
         *it = static_cast<char>( tolower( *it ) );
      }
      ++it;
   }
   return in_text;
}
// end of to_lower
