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

#ifndef WIN32
#include <stream.h>
#endif
#include "smplhist.h"
#include <cmath>
#ifndef HUGE_VAL
#ifdef HUGE
#define HUGE_VAL HUGE
#else
#include <float.h>
#define HUGE_VAL DBL_MAX
#endif
#endif

#include "IndentedTrace.h"

const int SampleHistogramMinimum = -2;
const int SampleHistogramMaximum = -1;
SampleHistogram::SampleHistogram(double low, double high, double width)
{
	IT_IT("SampleHistogram::SampleHistogram");
	
	if (high < low) {
		double t = high;
		high = low;
		low = t;
	}
	if (width == -1) {
		width = (high - low) / 10;
	}
	howManyBuckets = int((high - low) / width) + 2;
	bucketCount = new int[howManyBuckets];
	bucketLimit = new double[howManyBuckets];
	double lim = low;
	for (int i = 0; i < howManyBuckets; i++) {
		bucketCount[i] = 0;
		bucketLimit[i] = lim;
		lim += width;
	}
	bucketLimit[howManyBuckets-1] = HUGE_VAL;	/* from math.h */
}
SampleHistogram::~SampleHistogram()
{
	IT_IT("SampleHistogram::~SampleHistogram");
	
	if (howManyBuckets > 0) {
		delete bucketCount;
		delete bucketLimit;
	}
}
void
SampleHistogram::operator+=(double value)
{
	IT_IT("SampleHistogram::operator+=");

	int i;
	for (i = 0; i < howManyBuckets; i++) {
		if (value < bucketLimit[i]) break;
	}
	bucketCount[i]++;
	this->SampleStatistic::operator+=(value);
}
int
SampleHistogram::similarSamples(double d)
{
	IT_IT("SampleHistogram::similarSamples");

	int i;
	for (i = 0; i < howManyBuckets; i++) {
		if (d < bucketLimit[i]) return(bucketCount[i]);
	}
	return(0);
}
void
SampleHistogram::printBuckets(ostream& s)
{
	IT_IT("SampleHistogram::printBuckets");
	
	for(int i = 0; i < howManyBuckets; i++) {
		if (bucketLimit[i] >= HUGE_VAL) {
			s << "< max : " << bucketCount[i] << "\n";
			} else {
			s << "< " << bucketLimit[i] << " : " << bucketCount[i] << "\n";
		}
	}
}
void
SampleHistogram::reset()
{
	IT_IT("SampleHistogram::reset");
	
	this->SampleStatistic::reset();
	if (howManyBuckets > 0) {
		for (register int i = 0; i < howManyBuckets; i++) {
			bucketCount[i] = 0;
		}
	}
}

