/* src_cutter.c
 *
 *	(C) Copyright Dec  9 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*CUT foobar*/
void usage()
{
    puts(
	 "\n\tCopyright Dec  9 1995, Edmond J. Breen\n\n" 
	 "Src_cutter takes as input a list of file names,\n"
	 "such as '*.c' or '*.h', and outputs a 2nd sequence of\n"
	 "files. It searches each input file for lines of text\n"
	 "starting with      /*CUT outputFileName*/\n"
	 "and ending with    /*END CUT*/\n"

	 "\nFor Example:\n"
	 "  1.\t#include <stdio.h>\n"
	 "  2.\t\t/*CUT foobar*/\n"
	 "  3.\tmain()\n"
	 "  4.\t{\n"
	 "  5.\t    puts(\"Hello, world!\");\n"
	 "  6.\t}\n"
	 "  7.\t\t/*END CUT*/\n"
	 "\nWhen the above source is put through src_cutter, it will produce\n"
	 "the output file foobar, which will contain lines 3 to 6 "
	 "inclusively.\n"
	 "There is no limit to the number of cuttings that can be\n"
	 "extracted from one file. Cuttings cannot be nested.\n"
	 "If the file foobar had already existed, then  \n"
	 "its previous contents would have been erased.\n"
	 	 
	 "\nUsage:\n"
	 "\tsrc_cutter [-nw#] [-ddir] inputfiles\n"
	 "Options:\n"
	 "\t-n   insert line numbers in output.\n"
	 "\t-w#  defines the field width that the line number\n"
	 "\t     will be printed in. Default is -w3\n"
	 "\t-ddir specifies the output directory to store the\n"
	 "\t      cuttings. By default it is set to the current\n"
	 "\t      working directory.\n"
	 "Examples:\n"
	 "\t  1: src_cutter *.c *.h\n"
	 "\t  2: src_cutter -n src_cutter.c\n"
	 "\t  3: src_cutter -nw5 -dcuttings *.c\n"
	 
	 "\nNB. src_cutter should be run from the directory\n"
	 "where you want the cuttings to be placed.\n"
	 
	 );
}	 
/*END CUT*/

int process(char *path, char *fname, int linenos, int width)
{
    char buf[512], *cp, *ep;
    char dirFile[512];
    FILE *fp, *fpOut;
    long lineno = 0,outlineno;
    char fmt[20];

    

    fp = fopen(fname,"r");
    if(!fp) {
	fprintf(stderr,"failed to open %s\n",fname);
	return -1;
    }

    sprintf(fmt,"%%%dld. ",width);

    while(!feof(fp)) {
	fgets(buf,511,fp);
	lineno++;
	cp = buf;
	while(*cp && isspace(*cp))
	    cp++;
	if( *cp == '/' && (cp = strstr(buf,"/*CUT")) != NULL) {
	    cp += 5; /* increment past starting block */
	    while(*cp && isspace(*cp))
		cp++;
	    ep = cp;
	    while(*ep && *ep != '*')
		ep++;
	    if(!*ep) {
		fprintf(stderr,"Illegal line CUT line on line %ld"
			" in %s\n",lineno,fname);

		return -1;
	    }
	    *ep = 0;
	    if(*path) {
		dirFile[0] = 0;
		strcpy(dirFile,path);
		strcat(dirFile,"/");
		strcat(dirFile,cp);
	    } else
		strcpy(dirFile,cp);
	    fpOut = fopen(dirFile,"w");
	    if(!fpOut) {
		fprintf(stderr,"Failed to create file %s\n",dirFile);
		return -1;
	    }
	    outlineno = 0;
	    while(fgets(buf,511,fp)) {
		cp = buf;
		while(*cp && isspace(*cp))
		    cp++;
		if(*cp == '/' && strstr(buf,"/*END CUT"))
		    break;
		if(linenos) {
		    outlineno++;
		    fprintf(fpOut,fmt,outlineno);
		}
		fputs(buf,fpOut);
	    }
	    fclose(fpOut);
	    if(feof(fp)) {
		fprintf(stderr,"Unexpected end of file in"
			" %s\n",fname);
		return -1;
	    }
	}
    }
    return 1;
}


int LINENOS=0,FMTWIDTH=3;
char PATH[256] = {0};
int do_sw_commands(char *cp)
{
    int i;
    while(*cp) 
	switch(*cp++) {
	  case 'n': LINENOS=1;break;
	  case 'w': FMTWIDTH = atoi(cp);
	    while(*cp && (isspace(*cp) || isdigit(*cp)))
		cp++;
	    break;
	  case 'd':
	    for(i=0;*cp && !isspace(*cp);i++)
		PATH[i] = *cp++;
	    PATH[i] = 0;
	    break;
	  default:
	    return 0;
	}
    return 1;
}
	


void main(int argc, char **argv)
{
    if(argc == 1) {
	usage();
	return;
    }

    while(argv[1][0] == '-') {
	if(!do_sw_commands(&argv[1][1])) {
	    fprintf(stderr,"Unknown switch command\n");
	    exit(1);
	}
	argv++;
	argc--;
    }
    
    while(--argc > 0) {
	printf("%s\n",*++argv);
	if(process(PATH,*argv,LINENOS,FMTWIDTH) < 0)
	    return;
    }
}


