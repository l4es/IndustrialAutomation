/* string-lib.h - headers for string-lib.c
   $Id: string-lib.h,v 1.2 1998/05/05 19:43:00 edb Exp $
*/

char *newstr(char *str);
char *substr(char *str, int offset, int len);
char *replace_ltgt(char *str);
char *lower_case(char *buffer);
