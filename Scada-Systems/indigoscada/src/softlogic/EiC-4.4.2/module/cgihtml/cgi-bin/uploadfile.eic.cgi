#!/usr/local/bin/eic -f

#include "EiCpaths"
#include "string-lib.c"
#include "cgi-llist.c"
#include "cgi-lib.c"
#include "html-lib.c"

int checkFileName(char *f)
{
    /* returns 1 if filename is a single component.
     * else returns 0;
     */
    if(f) {
	while(*f)
	    if(*f++ == '/')
		return 0;
	return 1;
    } 
    return 0;
}	

llist entries;
int stats;
char filename[256] = "/tmp/";

FILE *fp1 =NULL;

html_header();
stats = read_cgi_input(&entries);

if(cgi_val(entries,"userfile")) {
    char *fname;
    
    printf("<h1>Date: %s <br> file : %s </h1><br>\n",
	   __DATE__,cgi_val(entries,"userfile"));

    fname = cgi_val(entries,"userfile");

    if(checkFileName(fname)) {
	strncat(filename,fname,sizeof(filename) - strlen(filename) -1);
	fp1  = fopen(filename,"r");

	if(fp1) { 
	    char buff[512];
	    printf("<br>\n");
	    while(fgets(buff,511,fp1))
		printf("%s<br>",buff);
	    fclose(fp1);
	} else 
	    printf("Sorry failed to open file %s<br>\n",fname); 
    } else
	printf("Illegal filename %s<br>",fname);
    
} else  
    printf("No upload file entered<br>\n");

list_clear(&entries);
exit(0);




	  










