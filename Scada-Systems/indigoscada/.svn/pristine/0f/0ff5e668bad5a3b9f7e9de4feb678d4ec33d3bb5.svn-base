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

#ifndef SampleHistogram_h
#define SampleHistogram_h 
#include "common.h"
#include <iostream.h>
#include "smplstat.h"
extern const int SampleHistogramMinimum;
extern const int SampleHistogramMaximum;
class SampleHistogram : public SampleStatistic 
{
	protected:
	int  howManyBuckets;
	int *bucketCount;
	double *bucketLimit;
	public:
	SampleHistogram(double low, double hi, double bucketWidth = -1.0);
	~SampleHistogram();
	virtual void reset();
	virtual void operator+=(double);
	int similarSamples(double);
	int buckets();
	double bucketThreshold(int i);
	int inBucket(int i);
	void printBuckets(ostream&);
};
inline int SampleHistogram:: buckets() { return(howManyBuckets); };
	inline double SampleHistogram:: bucketThreshold(int i) {
		if (i < 0 || i >= howManyBuckets)
		error("invalid bucket access");
		return(bucketLimit[i]);
	}
	inline int SampleHistogram:: inBucket(int i) {
		if (i < 0 || i >= howManyBuckets)
		error("invalid bucket access");
		return(bucketCount[i]);
	}
#endif
	
