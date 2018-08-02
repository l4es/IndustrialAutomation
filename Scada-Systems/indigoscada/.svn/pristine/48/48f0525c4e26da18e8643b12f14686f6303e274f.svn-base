#!/home/edb/bin/eic -f 

/* 
 EiC version of test.cgi.c - Uses functions print_cgi_env() and print_entries()
     to test CGI.

   Eugene Kim, eekim@fas.harvard.edu
   $Id: test.eic.cgi,v 1.1 1998/04/10 19:10:14 edb Exp $

   Copyright (C) 1996 Eugene Eric Kim
   All Rights Reserved
*/


#include "EiCpaths"
#include "string-lib.c"
#include "cgi-llist.c"
#include "cgi-lib.c"
#include "html-lib.c"

llist entries;
int sttus;

html_header();
html_begin("Test CGI");
h1("CGI Test Program");
printf("<hr>\n");
h2("CGI Environment Variables");
print_cgi_env();
sttus = read_cgi_input(&entries);
printf("<h2>Status = %d</h2>\n",sttus);
h2("CGI Entries");
print_entries(entries);
html_end();
list_clear(&entries);


