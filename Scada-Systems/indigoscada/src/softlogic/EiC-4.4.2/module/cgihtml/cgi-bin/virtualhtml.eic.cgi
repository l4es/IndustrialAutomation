#!/usr/local/bin/eic -f

int printf(char *fmt, ...);

printf("Content-type: text/html\n\n");

printf("<html>\n"
       "<head><title> Simple Virtual HTML </title> </head>\n"
       "<body>\n"
       "<h1>Virtual HTML</h1><hr>\n" 
       "Hey look, I just created this page virtually!!!\n");


printf("<p>Date:  %s <br> Current Time: %s\n",__DATE__,__TIME__); 

printf("</body></html>\n");



