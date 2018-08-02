#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define __P(x) x
#define _DIAGASSERT(x) assert(x)

#ifdef __weak_alias
__weak_alias(getopt,_getopt);
#endif


int	opterr = 1,		/* if error message should be printed */
	optind = 1,		/* index into parent argv vector */
	optopt,			/* character checked for validity */
	optreset;		/* reset getopt */
char	*optarg;		/* argument associated with option */

static char * _progname __P((char *));
int getopt_internal __P((int, char * const *, const char *));

static char *
_progname(nargv0)
	char * nargv0;
{
	char * tmp;

	_DIAGASSERT(nargv0 != NULL);

	tmp = strrchr(nargv0, '/');
	if (tmp)
		tmp++;
	else
		tmp = nargv0;
	return(tmp);
}

#define	BADCH	(int)'?'
#define	BADARG	(int)':'
#define	EMSG	""

/*
 * getopt --
 *	Parse argc/argv argument vector.
 */
int
getopt(nargc, nargv, ostr)
	int nargc;
	char * const nargv[];
	const char *ostr;
{
	static char *__progname = 0;
	static char *place = EMSG;		/* option letter processing */
	char *oli;				/* option letter list index */
        __progname = __progname?__progname:_progname(*nargv);

	_DIAGASSERT(nargv != NULL);
	_DIAGASSERT(ostr != NULL);

	if (optreset || !*place) {		/* update scanning pointer */
		optreset = 0;
		if (optind >= nargc || *(place = nargv[optind]) != '-') {
			place = EMSG;
			return (-1);
		}
		if (place[1] && *++place == '-'	/* found "--" */
		    && place[1] == '\0') {
			++optind;
			place = EMSG;
			return (-1);
		}
	}					/* option letter okay? */
	if ((optopt = (int)*place++) == (int)':' ||
	    !(oli = strchr(ostr, optopt))) {
		/*
		 * if the user didn't specify '-' as an option,
		 * assume it means -1.
		 */
		if (optopt == (int)'-')
			return (-1);
		if (!*place)
			++optind;
		if (opterr && *ostr != ':')
			(void)fprintf(stderr,
			    "%s: illegal option -- %c\n", __progname, optopt);
		return (BADCH);
	}
	if (*++oli != ':') {			/* don't need argument */
		optarg = NULL;
		if (!*place)
			++optind;
	}
	else {					/* need an argument */
		if (*place)			/* no white space */
			optarg = place;
		else if (nargc <= ++optind) {	/* no arg */
			place = EMSG;
			if (*ostr == ':')
				return (BADARG);
			if (opterr)
				(void)fprintf(stderr,
				    "%s: option requires an argument -- %c\n",
				    __progname, optopt);
			return (BADCH);
		}
	 	else				/* white space */
			optarg = nargv[optind];
		place = EMSG;
		++optind;
	}
	return (optopt);			/* dump back option letter */
}

