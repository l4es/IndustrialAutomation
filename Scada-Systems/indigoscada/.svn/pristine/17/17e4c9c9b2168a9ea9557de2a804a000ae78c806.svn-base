#!/usr/local/bin/eic -f 

/* query-results.c - generic query program using either GET or POST
   Eugene Kim, eekim@fas.harvard.edu


   Copyright (C) 1996 Eugene Eric Kim
   All Rights Reserved
*/

#include "EiCpaths"
#include "string-lib.c"
#include "cgi-llist.c"
#include "cgi-lib.c"
#include "html-lib.c"

llist entries;
int stats;

html_header();
html_begin("Query Results");
stats = read_cgi_input(&entries);
printf("<h1>Status = %d</h1>\n",stats);
h1("Query results");
print_entries(entries);
html_end();
list_clear(&entries);

exit(0);