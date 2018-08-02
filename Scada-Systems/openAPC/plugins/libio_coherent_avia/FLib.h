#ifndef __FLIB_INCLUDED__
#define __FLIB_INCLUDED__

#include <string>
#include <list>

#ifdef ENV_LINUX
 #include <dirent.h>
#endif

namespace Frewitt {
	
   using namespace std;
	
   class FLib {
   public:
      static const char TAB;
      static const char CR;
      static const char LF;
      static const unsigned char FNC1; // for GS1
      
//      static int kbhit();
//      static int getch();
      
      // sleep [ms]
      static void Sleep(const unsigned int delay);
            
      // remove char in the string
      static void Trim(string& text, const char);
      
      // remove CR at the end of the string
      static void TrimCR(string& text);
      
      // remove LF at the end of the string
      static void TrimLF(string& text);
      
      // remove CRLF at the end of the string
      static void TrimCRLF(string& text);
      
      // remove SPACE in the string (used for communication with PLC)
      static void TrimSP(string& text);

      // Chop string after char
      static void Chop(string& text, const char);
      static void ChopCR(string& text);
      static void ChopLF(string& text);
      static void ChopCRLF(string& text);

      // execute command line
      static void Execute(const string& command);
      
#ifdef ENV_LINUX // dirent.h not available on windows and directory functions are not needed...
      // directory handling functions
      static DIR* OpenDir(const string& sdir);
      static void CloseDir(DIR *dir);
      static list<string> ListDir(const string& sdir);
      static string ListNextFile(DIR* dir);
      static bool DirectoryExists(const string& sdir);
#endif

   private:
      FLib();
      ~FLib();
      FLib(FLib&);
   }; // class
	
} // namespace

#endif // __FLIB_INCLUDED__
