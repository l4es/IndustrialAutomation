/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#include "smplstat.h"
#include <cmath>

#ifndef HUGE_VAL
#ifdef HUGE
#define HUGE_VAL HUGE
#else
#include <float.h>
#define HUGE_VAL DBL_MAX
#endif
#endif
void SampleStatistic::error(const char* s) // args used
{
//	IT_IT("SampleStatistic::error");
	
	throw s;
}
// t-distribution: given p-value and degrees of freedom, return t-value
// adapted from Peizer & Pratt JASA, vol63, p1416
double tval(double p, int df)
{
//	IT_IT("tval");
	
	double t;
	int positive = p >= 0.5;
	p = (positive)? 1.0 - p : p;
	if (p <= 0.0 || df <= 0)
	t = HUGE_VAL;
	else if (p == 0.5)
	t = 0.0;
	else if (df == 1)
	t = 1.0 / tan((p + p) * 1.57079633);
	else if (df == 2)
	t = sqrt(1.0 / ((p + p) * (1.0 - p)) - 2.0);
	else
	{
		double ddf = df;
		double a = sqrt(log(1.0 / (p * p)));
		double aa = a * a;
		a = a - ((2.515517 + (0.802853 * a) + (0.010328 * aa)) /
		(1.0 + (1.432788 * a) + (0.189269 * aa) +
		(0.001308 * aa * a)));
		t = ddf - 0.666666667 + 1.0 / (10.0 * ddf);
		t = sqrt(ddf * (exp(a * a * (ddf - 0.833333333) / (t * t)) - 1.0));
	}
	return (positive)? t : -t;
}
void
SampleStatistic::reset()
{
//	IT_IT("SampleStatistic::reset");
	
	n = 0; x = x2 = 0.0;
	maxValue = -HUGE_VAL;
	minValue = HUGE_VAL;
}
void
SampleStatistic::operator+=(double value)
{
//	IT_IT("SampleStatistic::operator+=");

	n += 1;
	x += value;
	x2 += (value * value);
	if ( minValue > value) minValue = value;
	if ( maxValue < value) maxValue = value;
}
double
SampleStatistic::mean() const
{
//	IT_IT("SampleStatistic::mean");
	
	if ( n > 0) {
		return (x / n);
	}
	else {
		return ( 0.0 );
	}
}
double
SampleStatistic::var() const
{
//	IT_IT("SampleStatistic::var()");
	
	if ( n > 1) {
		return(( x2 - ((x * x) /  n)) / ( n - 1));
	}
	else {
		return ( 0.0 );
	}
}
double
SampleStatistic::stdDev() const
{
//	IT_IT("SampleStatistic::stdDev");
	
	if ( n <= 0 || this -> var() <= 0) {
		return(0);
		} else {
		return( (double) sqrt( var() ) );
	}
}
double
SampleStatistic::confidence(int interval) const
{
//	IT_IT("SampleStatistic::confidence(int interval)");
	
	int df = n - 1;
	if (df <= 0) return HUGE_VAL;
	double t = tval(double(100 + interval) * 0.005, df);
	if (t == HUGE_VAL)
	return t;
	else
	return (t * stdDev()) / sqrt(double(n));
}
double
SampleStatistic::confidence(double p_value) const
{
//	IT_IT("SampleStatistic::confidence(double p_value)");
	
	int df = n - 1;
	if (df <= 0) return HUGE_VAL;
	double t = tval((1.0 + p_value) * 0.5, df);
	if (t == HUGE_VAL)
	return t;
	else
	return (t * stdDev()) / sqrt(double(n));
}

