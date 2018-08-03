#ifndef INCLUDED_BtToken_h
#define INCLUDED_BtToken_h

//#define TEST

/*------- include files:
-------------------------------------------------------------------*/
#include <string>
#include <vector>

/*------- class declaration:
-------------------------------------------------------------------*/
class BtToken
{
//******* CONSTRUCTION / DESTRUCTION *******
public:
   BtToken()
   : position_ ( int() )
   , length_   ( int() )
#ifdef TEST
   , text_     ( std::string() )
#endif
   , hash_     ( 0     )
   {}
   BtToken( const int in_position, const int in_length, const std::string& in_text )
   : position_ ( in_position )
   , length_   ( in_length   )
#ifdef TEST
   , text_     ( in_text     )
#endif
   , hash_     ( hash( in_text ) )
   {}
   BtToken( const BtToken& rhs )
   : position_ ( rhs.position_ )
   , length_   ( rhs.length_   )
#ifdef TEST
   , text_     ( rhs.text_     )
#endif
   , hash_     ( rhs.hash_     )
   {}
   BtToken& operator=( const BtToken& rhs )
   {
      if( this != &rhs ) {
         position_ = rhs.position_;
         length_   = rhs.position_;
#ifdef TEST
         text_     = rhs.text_;
#endif
         hash_     = rhs.hash_;
      }
      return *this;
   }

//******* MEMBERS *******
private:
   int position_;
   int length_;
#ifdef TEST
   std::string  text_;
#endif
   unsigned int hash_;

//******* METHODS *******
public:
   int                position() const { return position_; }
   int                length  () const { return length_;   }
   unsigned int       hash    () const { return hash_;     }
#ifdef TEST
   const std::string& text    () const { return text_;     }
#endif

private:
   //======= hash =======
   unsigned int hash( const std::string& in_text ) {
      unsigned int hash = 0xAAAAAAAA;
      const unsigned int n = in_text.length();
      for( unsigned int i = 0; i < n; i++ ) {
         hash ^= ( (i & 1) == 0)
               ? (   ( hash <<  7) ^ in_text[i] ^ (hash >> 3 ) )
               : (~( ( hash << 11) ^ in_text[i] ^ (hash >> 5 ) ) );
      }
   
      return hash;
   }
};


/*------- global types:
-------------------------------------------------------------------*/
typedef std::vector<BtToken> VTokens;

#endif // INCLUDED_BtToken_h
