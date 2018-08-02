/* +++Date last modified: 05-Jul-1997 */

/*
**  MATHSTAT.H - Header file for statistical analysis in C and C++
**
**  Public domain by Bob Stout
*/

#include <stddef.h>
#include <math.h>
#include "sniptype.h"

#if !(__cplusplus)

/*
**  Structure to hold statistical analysis data
**
**  total   = Total of all data added
**  total2  = Total of squares of all data added
**  product = Product of all data added
**  recip   = Total of the reciprocals of all data added
**  count   = Number of data elements under analysis
**  min1    = Minimum datum to date
**  min2    = Next to lowest datum to date
**  oldmin  = Previous value of min1
**  max1    = Maximum datum to date
**  max2    = Next to highest datum to date
**  oldmax  = Previous value of max1
**
**  Notes: min2, oldmin, max2, and oldmax are used when deleting data.
**         If the datum to be deleted is either the highest or lowest to
**         date, it must be replaced with either the previous min/max
**         value (oldmin/oldmax) or the penultimate min/max value (min2/max2).
**         Replacement with the penultimate value is indicated when the
**         previous min/max value was either DBL_MAX or DBL_MIN,
**         respectively, indicating the initialized condition.
**
**	     The C++ Stat class works identically to the functions used with
**	     the Stat_T structure in C. All members of Stat_T are present as
**	     private data objects in the Stat class. All functions associated
**	     with Stat_T are duplicated as member functions of class Stat.
**
**         Note that the data are not saved, therefore it is impossible to
**         provide functions such as one to return the median of a data set.
*/

typedef struct {
      size_t count;
      double total;
      double total2;
      double product;
      double recip;
      double min1, min2, oldmin;
      double max1, max2, oldmax;
} Stat_T;

void        stat_init(Stat_T *ptr);
size_t      stat_count(Stat_T *ptr);
size_t      stat_add(double datum, Stat_T *ptr);
size_t      stat_delete(double datum, Stat_T *ptr);
Boolean_T   stat_olympic(Stat_T *ptr);
double      stat_min(Stat_T *ptr);
double      stat_max(Stat_T *ptr);
double      stat_minerror(Stat_T *ptr);
double      stat_maxerror(Stat_T *ptr);
double      stat_mean(Stat_T *ptr);
double      stat_gmean(Stat_T *ptr);
double      stat_hmean(Stat_T *ptr);
double      stat_stddevP(Stat_T *ptr);
double      stat_stddevS(Stat_T *ptr);
double      stat_var(Stat_T *ptr);
double      stat_varcoeffP(Stat_T *ptr);
double      stat_varcoeffS(Stat_T *ptr);

#else /* is C++ */

class Stat {
      private:
            double            total;
            double            total2;
            double            product;
            double            recip;
            size_t            count;
            double            min1, min2, oldmin;
            double            max1, max2, oldmax;
      public:
            inline            Stat();
            inline size_t     Count();
            size_t            Add(double datum);
            size_t            Delete(double datum);
            Boolean_T         Olympic();
            inline double     Min();
            inline double     Max();
            double            Minerror();
            double            Maxerror();
            double            Mean();
            double            Gmean();
            double            Hmean();
            double            StddevP();
            double            StddevS();
            inline double     Var();
            double            VarcoeffP();
            double            VarcoeffS();
};

#endif
