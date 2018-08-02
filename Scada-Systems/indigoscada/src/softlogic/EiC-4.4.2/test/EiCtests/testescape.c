#include <stdio.h>

int main () {

  printf ("audible alert (bell) BEL  \\a   %d\n" , '\a');
  printf ("backspace            BS   \\b   %d\n" , '\b');
  printf ("horizontal tab       HT   \\t   %d\n" , '\t');
  printf ("newline              LF   \\n  %d\n"  , '\n');
  printf ("vertical tab         VT   \\v  %d\n"  , '\v');
  printf ("formfeed             FF   \\f  %d\n"  , '\f');
  printf ("carriage return      CR   \\r  %d\n"  , '\r');
  printf ("double quote         \"    \\\"  %d\n", '\"');
  printf ("single quote         \'    \\\'  %d\n", '\'');
  printf ("question mark        ?    \\?  %d\n"  , '\?');
  printf ("backslash            \\    \\\\  %d\n", '\\');

  return 0;
}

#ifdef EiCTeStS
main();
#endif

