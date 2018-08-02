/* +++Date last modified: 05-Jul-1997 */

/*
**  MATHSTAT.C - Statistical analysis in C and C++
**
**  Public domain by Bob Stout
*/

#include <stddef.h>
#include <float.h>
#include "mathstat.h"

/*
**  Initialize a stat structure
*/

#if !(__cplusplus)

void stat_init(Stat_T *ptr)
{
      ptr->count   = 0;
      ptr->total   = 0.0;
      ptr->total2  = 0.0;
      ptr->recip   = 0.0;
      ptr->product = 1.0;
      ptr->min1 = ptr->min2 = ptr->oldmin = DBL_MAX;
      ptr->max1 = ptr->max2 = ptr->oldmax = DBL_MIN;
}

#else /* C++ */

inline Stat::Stat()
{
      count   = 0;
      total   = 0.0;
      total2  = 0.0;
      recip   = 0.0;
      product = 1.0;
      min1 = min2 = oldmin = DBL_MAX;
      max1 = max2 = oldmax = DBL_MIN;
}

#endif

/*
**  Return the number of data points
*/

#if !(__cplusplus)

size_t stat_count(Stat_T *ptr)
{
      return ptr->count;
}

#else /* C++ */

inline size_t Stat::Count()
{
      return count;
}

#endif

/*
**  Add a value for statistical analysis
*/

#if !(__cplusplus)

size_t stat_add(double datum, Stat_T *ptr)
{
      ptr->total   += datum;
      ptr->total2  += (datum * datum);
      ptr->recip   += 1.0 / datum;
      ptr->product *= datum;
      ++(ptr->count);
      if (datum < ptr->min1)
      {
            ptr->oldmin = ptr->min1;
            ptr->min1 = datum;
      }
      else if (datum < ptr->min2)
            ptr->min2 = datum;
      if (datum > ptr->max1)
      {
            ptr->oldmax = ptr->max1;
            ptr->max1 = datum;
      }
      else if (datum < ptr->max2)
            ptr->max2 = datum;
      return ptr->count;
}

#else /* C++ */

size_t Stat::Add(double datum)
{
      total   += datum;
      total2  += (datum * datum);
      recip   += 1.0 / datum;
      product *= datum;
      ++(count);
      if (datum < min1)
      {
            oldmin = min1;
            min1 = datum;
      }
      else if (datum < min2)
            min2 = datum;
      if (datum > max1)
      {
            oldmax = max1;
            max1 = datum;
      }
      else if (datum < max2)
            max2 = datum;
      return count;
}

#endif

/*
**  Delete a value from a statistical analysis
*/

#if !(__cplusplus)

size_t stat_delete(double datum, Stat_T *ptr)
{
      ptr->total   -= datum;
      ptr->total2  -= (datum * datum);
      ptr->recip   -= 1.0 / datum;
      ptr->product /= datum;
      --(ptr->count);
      if (datum == ptr->min1)
            ptr->min1 = (DBL_MAX == ptr->oldmin) ? ptr->min2 : ptr->oldmin;
      if (datum == ptr->max1)
            ptr->max1 = (DBL_MIN == ptr->oldmax) ? ptr->max2 : ptr->oldmax;
      return ptr->count;
}

#else /* C++ */

size_t Stat::Delete(double datum)
{
      total   -= datum;
      total2  -= (datum * datum);
      recip   -= 1.0 / datum;
      product /= datum;
      --(count);
      if (datum == min1)
            min1 = (DBL_MAX == oldmin) ? min2 : oldmin;
      if (datum == max1)
            max1 = (DBL_MIN == oldmax) ? max2 : oldmax;
      return count;
}

#endif

/*
**  "Olympic filter" - toss out the high and low data
*/

#if !(__cplusplus)

Boolean_T stat_olympic(Stat_T *ptr)
{
      if (ptr->count < 3)
            return Error_;
      stat_delete(ptr->min1, ptr);
      stat_delete(ptr->max1, ptr);
      return Success_;
}

#else /* C++ */

Boolean_T Stat::Olympic()
{
      if (count < 3)
            return Error_;
      Delete(min1);
      Delete(max1);
      return Success_;
}

#endif

/*
**  Return the minimum datum
*/

#if !(__cplusplus)

double stat_min(Stat_T *ptr)
{
      return ptr->min1;
}

#else /* C++ */

inline double Stat::Min()
{
      return min1;
}

#endif

/*
**  Return the maximum datum
*/

#if !(__cplusplus)

double stat_max(Stat_T *ptr)
{
      return ptr->max1;
}

#else /* C++ */

inline double Stat::Max()
{
      return max1;
}

#endif

/*
**  Return the error (%) for the minimum datum
*/

#if !(__cplusplus)

double stat_minerror(Stat_T *ptr)
{
      double Mean = stat_mean(ptr);

      return 100.0 * ((ptr->min1 - Mean) / Mean) ;
}

#else /* C++ */

double Stat::Minerror()
{
      double mean_ = Mean();

      return 100.0 * ((min1 - mean_) / mean_) ;
}

#endif

/*
**  Return the error (%) for the maximum datum
*/

#if !(__cplusplus)

double stat_maxerror(Stat_T *ptr)
{
      double Mean = stat_mean(ptr);

      return 100.0 * ((ptr->max1 - Mean) / Mean) ;
}

#else /* C++ */

double Stat::Maxerror()
{
      double mean_ = Mean();

      return 100.0 * ((max1 - mean_) / mean_) ;
}

#endif

/*
**  Compute the arithmetic mean
*/

#if !(__cplusplus)

double stat_mean(Stat_T *ptr)
{
      if (ptr->count)
            return (ptr->total / ptr->count);
      else  return 0.0;
}

#else /* C++ */

double Stat::Mean()
{
      if (count)
            return (total / count);
      else  return 0.0;
}

#endif

/*
**  Compute the geometric mean
*/

#if !(__cplusplus)

double stat_gmean(Stat_T *ptr)
{
      if (ptr->count)
            return pow(ptr->product, 1.0 / ptr->count);
      else  return 0.0;
}

#else /* C++ */

double Stat::Gmean()
{
      if (count)
            return pow(product, 1.0 / count);
      else  return 0.0;
}

#endif

/*
**  Compute the harmonic mean
*/

#if !(__cplusplus)

double stat_hmean(Stat_T *ptr)
{
      if (ptr->count)
            return ptr->count / ptr->recip;
      else  return 0.0;
}

#else /* C++ */

double Stat::Hmean()
{
      if (count)
            return count / recip;
      else  return 0.0;
}

#endif

/*
**  Compute the standard deviation of the population
*/

#if !(__cplusplus)

double stat_stddevP(Stat_T *ptr)
{
      double tmp;

      tmp  = ptr->total / ptr->count;
      tmp *= tmp;
      return sqrt((ptr->total2 / ptr->count) - tmp);
}

#else /* C++ */

double Stat::StddevP()
{
      double tmp;

      tmp  = total / count;
      tmp *= tmp;
      return sqrt((total2 / count) - tmp);
}

#endif

/*
**  Compute the standard deviation of the sampled data
*/

#if !(__cplusplus)

double stat_stddevS(Stat_T *ptr)
{
      double mean, total, total2, N, tmp;

      if (ptr->count < 2)
            return 0.0;

      mean   = ptr->total / ptr->count;
      total  = ptr->total - mean;
      total2 = ptr->total2 - (mean * mean);
      N      = ptr->count - 1;
      tmp    = total / N;
      tmp   *= tmp;
      return sqrt((total2 / N) - tmp);
}

#else /* C++ */

double Stat::StddevS()
{
      double Mean, Total, Total2, N, tmp;

      if (count < 2)
            return 0.0;

      Mean   = total / count;
      Total  = total - Mean;
      Total2 = total2 - (Mean * Mean);
      N      = count - 1;
      tmp    = Total / N;
      tmp   *= tmp;
      return sqrt((Total2 / N) - tmp);
}

#endif

/*
**  Compute the variance
*/

#if !(__cplusplus)

double stat_var(Stat_T *ptr)
{
      return stat_stddevS(ptr) * stat_stddevS(ptr);
}

#else /* C++ */

inline double Stat::Var()
{
      return StddevS() * StddevS();
}

#endif

/*
**  Compute the coefficient of variation (percentage) for the population
*/

#if !(__cplusplus)

double stat_varcoeffP(Stat_T *ptr)
{
      return (stat_stddevP(ptr) / stat_mean(ptr)) * 100.0;
}

#else /* C++ */

double Stat::VarcoeffP()
{
      return (StddevP() / Mean()) * 100.0;
}

#endif

/*
**  Compute the coefficient of variation (percentage) for the sample
*/

#if !(__cplusplus)

double stat_varcoeffS(Stat_T *ptr)
{
      return (stat_stddevS(ptr) / stat_mean(ptr)) * 100.0;
}

#else /* C++ */

double Stat::VarcoeffS()
{
      return (StddevS() / Mean()) * 100.0;
}

#endif


/*
**  Test harness begins here. Define TEST macro to build stand-alone.
*/

#ifdef TEST

#include <stdlib.h>

#if !(__cplusplus)

#include <stdio.h>

int main(int argc, char **argv)
{
      Stat_T data;

      stat_init(&data);
      while (--argc)
      {
            double ftmp;

            ftmp = atof(*(++argv));
            stat_add(ftmp, &data);
      }
      puts("\nBefore \"Olympic\" filtering\n");
      printf("Minimum datum             = %g\n", stat_min(&data));
      printf("Maximum datum             = %g\n", stat_max(&data));
      printf("Number of samples         = %d\n", stat_count(&data));
      printf("Arithmetic mean           = %g\n", stat_mean(&data));
      printf("Geometric mean            = %g\n", stat_gmean(&data));
      printf("Harmonic mean             = %g\n", stat_hmean(&data));
      printf("Standard deviation (N)    = %g\n", stat_stddevP(&data));
      printf("Standard deviation (N-1)  = %g\n", stat_stddevS(&data));
      printf("Variance                  = %g\n", stat_var(&data));
      printf("Population coeff. of var. = %g%%\n", stat_varcoeffP(&data));
      printf("Sample coeff. of var.     = %g%%\n", stat_varcoeffS(&data));

      puts("\nAfter \"Olympic\" filtering\n");
      printf("stat_olympic() returned %s\n", stat_olympic(&data) ?
            "ERROR" : "SUCCESS");
      printf("Minimum datum             = %g\n", stat_min(&data));
      printf("Maximum datum             = %g\n", stat_max(&data));
      printf("Number of samples         = %d\n", stat_count(&data));
      printf("Arithmetic mean           = %g\n", stat_mean(&data));
      printf("Geometric mean            = %g\n", stat_gmean(&data));
      printf("Harmonic mean             = %g\n", stat_hmean(&data));
      printf("Standard deviation (N)    = %g\n", stat_stddevP(&data));
      printf("Standard deviation (N-1)  = %g\n", stat_stddevS(&data));
      printf("Variance                  = %g\n", stat_var(&data));
      printf("Population coeff. of var. = %g%%\n", stat_varcoeffP(&data));
      printf("Sample coeff. of var.     = %g%%\n", stat_varcoeffS(&data));

      return EXIT_SUCCESS;
}

#else /* C++ */

#include <iostreams.h>

int main(int argc, char *argv[])
{
      class Stat data;
      char *str;

      while (--argc)
      {
            double ftmp;

            ftmp = atof(*(++argv));
            data.Add(ftmp);
      }
      cout << endl << "Before \"Olympic\" filtering\n" << endl << endl;
      cout << "Minimum datum             = " << data.Min()       << endl;
      cout << "Maximum datum             = " << data.Max()       << endl;
      cout << "Number of samples         = " << data.Count()     << endl;
      cout << "Arithmetic mean           = " << data.Mean()      << endl;
      cout << "Geometric mean            = " << data.Gmean()     << endl;
      cout << "Harmonic mean             = " << data.Hmean()     << endl;
      cout << "Standard deviation (N)    = " << data.StddevP()   << endl;
      cout << "Standard deviation (N-1)  = " << data.StddevS()   << endl;
      cout << "Variance                  = " << data.Var()       << endl;
      cout << "Population coeff. of var. = " << data.VarcoeffP() << "%" << endl;
      cout << "Sample coeff. of var.     = " << data.VarcoeffS() << "%" << endl;

      if (Success_ == data.Olympic())
            str = "SUCCESS";
      else  str = "ERROR";

      cout << endl << "After \"Olympic\" filtering" << endl << endl;
      cout << "data.Olympic() returned " << str << endl;
      cout << "Minimum datum             = " << data.Min()       << endl;
      cout << "Maximum datum             = " << data.Max()       << endl;
      cout << "Number of samples         = " << data.Count()     << endl;
      cout << "Arithmetic mean           = " << data.Mean()      << endl;
      cout << "Geometric mean            = " << data.Gmean()     << endl;
      cout << "Harmonic mean             = " << data.Hmean()     << endl;
      cout << "Standard deviation (N)    = " << data.StddevP()   << endl;
      cout << "Standard deviation (N-1)  = " << data.StddevS()   << endl;
      cout << "Variance                  = " << data.Var()       << endl;
      cout << "Population coeff. of var. = " << data.VarcoeffP() << "%" << endl;
      cout << "Sample coeff. of var.     = " << data.VarcoeffS() << "%" << endl;

      return EXIT_SUCCESS;
}

#endif /* C/C++ */

#endif
