#ifndef INCLUDED_BtStringTool_h
#define INCLUDED_BtStringTool_h

/*------- include files:
-------------------------------------------------------------------*/
#include <string>
#include <list>

/*------- class declaration:
-------------------------------------------------------------------*/
class BtStringTool
{
//******* CONSTANTS *******
private:
   static const std::string WhiteSpaces;

//******* METHODS *******
public:
   bool         is_white_space ( const char );
   bool         is_white_string( const std::string& );
   void         split          ( const std::string&, std::list<std::string>&, const char = ' ' );
   std::string& to_lower       ( std::string& ) const;

   //======= ltrim =======
   std::string ltrim ( const std::string& in_text ) const {
      const unsigned int pos = in_text.find_first_not_of( ' ' );
      return ( ( pos == std::string::npos ) ? in_text : in_text.substr( pos ) );
   }

   //======= rtrim =======
   std::string rtrim ( const std::string& in_text ) const {
      return in_text.substr( 0, in_text.find_last_not_of( ' ' ) + 1 );
   }

   //======= trim =======
   std::string trim( const std::string& in_text ) const {
      return ltrim( rtrim( in_text ) );
   }
};


#endif // INCLUDED_BtStringTool_h
