/********************************************************************
statistics.h
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#ifndef STATISTICS_H
#define STATISTICS_H

#include <string.h>
#include <math.h>
#include "lapacke.h"

#ifdef __cplusplus
extern "C"{
#endif

double mean(double* series, unsigned int size);

double variance(double* series, unsigned int size, double mean);

double autocovariance(double* series, unsigned int size, unsigned int lag, double mean);

double covariance(double* series_a, double* series_b, unsigned int size, unsigned int lag, double mean_a, double mean_b);

double autocorrelation(double* series, unsigned int size, unsigned int lag, double mean);

double correlation(double* series_a, double* series_b, unsigned int size, unsigned int lag, double mean_a, double mean_b);

void autocorrelation_function(double* series, unsigned int size, unsigned int num_lags, double mean, double* acf);

double partial_autocorrelation(double* series, unsigned int size, unsigned int lag, double mean);

void partial_autocorrelation_function(double* series, unsigned int size, unsigned int num_lags, double mean, double* pacf);

void difference(double* series, unsigned int size, unsigned int d, double* differenced);

void undifference(double* series, unsigned int size, unsigned int d, double* pre_sample_series);

void detrend(double* series, unsigned int size, double* alpha, double* beta, double* residuals);

unsigned int factorial(unsigned int n);

unsigned int binomial(unsigned int n, unsigned int k);

double aic(unsigned int size, double rss, unsigned int k);

double bic(unsigned int size, double rss, unsigned int k);

#ifdef __cplusplus
}
#endif

#endif
