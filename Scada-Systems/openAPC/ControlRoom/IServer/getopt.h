#ifndef _GETOPT_H

#define _GETOPT_H 1

#include <ctype.h>

#ifndef __THROW
# ifndef __GNUC_PREREQ
#  define __GNUC_PREREQ(maj, min) (0)
# endif
# if defined __cplusplus && __GNUC_PREREQ (2,8)
#  define __THROW	throw ()
# else
#  define __THROW
# endif
#endif

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;


struct option
{
  const char *name;
  /* has_arg can't be an enum because some compilers complain about
     type mismatches in all the code that assumes it is an int.  */
  int has_arg;
  int *flag;
  int val;
};

/* Names for the values of the `has_arg' field of `struct option'.  */

# define no_argument		0
# define required_argument	1
# define optional_argument	2


#ifdef __GNU_LIBRARY__
/* Many other libraries have conflicting prototypes for getopt, with
   differences in the consts, in stdlib.h.  To avoid compilation
   errors, only prototype getopt for the GNU C library.  */
extern int getopt (int ___argc, char *const *___argv, const char *__shortopts)
       __THROW;
#else /* not __GNU_LIBRARY__ */
extern int getopt ();
#endif /* __GNU_LIBRARY__ */

struct _getopt_data;

extern int getopt_long (int ___argc, char *const *___argv,
			char *__shortopts,
		        const struct option *__longopts, int *__longind)
       __THROW;
extern int getopt_long_only (int ___argc, char *const *___argv,
			     char *__shortopts,
		             const struct option *__longopts, int *__longind)
       __THROW;

extern int
_getopt_internal_r (int argc, char *const *argv,char *optstring,
		    const struct option *longopts, int *longind,
		    int long_only, struct _getopt_data *d);


/* Reentrant versions which can handle parsing multiple argument
   vectors at the same time.  */

  enum
    {
      REQUIRE_ORDER, PERMUTE, RETURN_IN_ORDER
    };


/* Data type for reentrant functions.  */
struct _getopt_data
{
  /* These have exactly the same meaning as the corresponding global
     variables, except that they are used for the reentrant
     versions of getopt.  */
  int optind;
  int opterr;
  int optopt;
  char *optarg;

  /* Internal members.  */

  /* True if the internal members have been initialized.  */
  int __initialized;

  /* The next char to be scanned in the option-element
     in which the last option character we returned was found.
     This allows us to pick up the scan where we left off.

     If this is zero, or a null string, it means resume the scan
     by advancing to the next ARGV-element.  */
  char *__nextchar;

  /* Describe how to deal with options that follow non-option ARGV-elements.

     If the caller did not specify anything,
     the default is REQUIRE_ORDER if the environment variable
     POSIXLY_CORRECT is defined, PERMUTE otherwise.

     REQUIRE_ORDER means don't recognize them as options;
     stop option processing when the first non-option is seen.
     This is what Unix does.
     This mode of operation is selected by either setting the environment
     variable POSIXLY_CORRECT, or using `+' as the first character
     of the list of option characters.

     PERMUTE is the default.  We permute the contents of ARGV as we
     scan, so that eventually all the non-options are at the end.
     This allows options to be given in any order, even with programs
     that were not written to expect this.

     RETURN_IN_ORDER is an option available to programs that were
     written to expect options and other ARGV-elements in any order
     and that care about the ordering of the two.  We describe each
     non-option ARGV-element as if it were the argument of an option
     with character code 1.  Using `-' as the first character of the
     list of option characters selects this mode of operation.

     The special argument `--' forces an end of option-scanning regardless
     of the value of `ordering'.  In the case of RETURN_IN_ORDER, only
     `--' can cause `getopt' to return -1 with `optind' != ARGC.  */

  long __ordering;

  /* If the POSIXLY_CORRECT environment variable is set.  */
  int __posixly_correct;


  /* Handle permutation of arguments.  */

  /* Describe the part of ARGV that contains non-options that have
     been skipped.  `first_nonopt' is the index in ARGV of the first
     of them; `last_nonopt' is the index after the last of them.  */

  int __first_nonopt;
  int __last_nonopt;

#if defined _LIBC && defined USE_NONOPTION_FLAGS
  int __nonoption_flags_max_len;
  int __nonoption_flags_len;
# endif
};

/* The initializer is necessary to set OPTIND and OPTERR to their
   default values and to clear the initialization flag.  */
#define _GETOPT_DATA_INITIALIZER	{ 1, 1 }

extern int _getopt_internal_r (int argc, char *const *argv,char *optstring,
		    const struct option *longopts, int *longind,
		    int long_only, struct _getopt_data *d);

extern int _getopt_long_r (int ___argc, char *const *___argv,
			   const char *__shortopts,
			   const struct option *__longopts, int *__longind,
			   struct _getopt_data *__data);

extern int _getopt_long_only_r (int ___argc, char *const *___argv,
				const char *__shortopts,
				const struct option *__longopts,
				int *__longind,
				struct _getopt_data *__data);

#endif /* getopt.h */
