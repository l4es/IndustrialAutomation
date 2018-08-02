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

#ifndef SampleStatistic_h
#define SampleStatistic_h 1
#include "common.h"
#include <stdlib.h>
#include <string.h>
class QSEXPORT SampleStatistic {
	protected:
	int	n;
	double x;
	double x2;
	double minValue, maxValue;
	public :
	SampleStatistic();
	virtual ~SampleStatistic();
	virtual void reset(); 
	SampleStatistic & operator = (const SampleStatistic &s) // copy operator
	{
		n = s.n;
		x = s.x;
		x2 = s.x2;
		minValue = s.minValue;
		maxValue = s.maxValue;
		return *this;
	};
	void set(int i, double sx, double sx2, double mn, double mx)
	{
		n = i;
		x = sx;
		x2 = sx2;
		minValue = mn;
		maxValue = mx;
	};   
	virtual void operator+=(double);
	int  samples() const;
	double mean() const;
	double stdDev() const;
	double var() const;
	double Min() const { return minValue;};
	double Max() const { return maxValue;};
	double confidence(int p_percentage) const;
	double confidence(double p_value) const;
	void error(const char* msg);
	double sum() const { return  x;};
	double sum2() const {return x2;};
};
inline SampleStatistic:: SampleStatistic(){ reset();}
	inline int SampleStatistic::  samples() const {return(n);}
	inline SampleStatistic::~SampleStatistic() {}
	#endif
	
