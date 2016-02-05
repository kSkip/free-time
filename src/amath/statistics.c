/********************************************************************
statistics.c
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include "amath/statistics.h"

double mean(double* series, unsigned int size){

	double mean = 0;
	unsigned int i;

	for(i=0;i<size;i++) mean += series[i];

	mean /= size;

	return mean;

}

double variance(double* series, unsigned int size, double mean){

	return autocovariance(series,size,0,mean);

}

double autocovariance(double* series, unsigned int size, unsigned int lag, double mean){

	double covariance = 0;
	unsigned int i, count = 0;

	for(i=0;i<size;i++){

		if((i+lag) < size){

			covariance += (series[i]-mean)*(series[i+lag]-mean);
			count++;

		}

	}

	covariance /= count-1;

	return covariance;
}

double covariance(double* series_a,
                  double* series_b,
                  unsigned int size,
                  unsigned int lag,
                  double mean_a,
                  double mean_b)
{

	double covariance = 0;
	unsigned int i, count = 0;

	for(i=0;i<size;i++){

		if((i+lag) < size){

			covariance += (series_a[i]-mean_a)*(series_b[i+lag]-mean_b);
			count++;

		}

	}

	covariance /= count-1;

	return covariance;

}

double autocorrelation(double* series, unsigned int size, unsigned int lag, double mean){

	double var = variance(series,size,mean);

	double covar = autocovariance(series,size,lag,mean);

	return covar / var;

}

double correlation(double* series_a,
                   double* series_b,
                   unsigned int size,
                   unsigned int lag,
                   double mean_a,
                   double mean_b)
{

	double var_a = variance(series_a,size,mean_a);
	double var_b = variance(series_b,size,mean_b);

	double covar = covariance(series_a,series_b,size,lag,mean_a,mean_b);

	return covar / sqrt(var_a * var_b);

}

void autocorrelation_function(double* series, unsigned int size, unsigned int num_lags, double mean, double* acf){

	unsigned int i;

	for(i=0;i<num_lags;i++) acf[i] = autocorrelation(series, size, i+1, mean);

}

double partial_autocorrelation(double* series, unsigned int size, unsigned int lag, double mean){

	int i, j;

	double* ac = (double*)malloc(lag*sizeof(double));
	double* A = (double*)malloc(lag*lag*sizeof(double));
	double* b = (double*)malloc(lag*sizeof(double));

	int* ipiv = (int*)malloc(lag*sizeof(int));

	for(i=0;i<(int)lag;i++){

		ac[i] = autocorrelation(series, size, i+1, mean);
		b[i] = ac[i];

	}

	for(i=0;i<(int)lag;i++){


		for(j=0;j<(int)lag;j++){
			if(i==j){
				A[lag*i+j] = 1.0f;
			}else{
				A[lag*i+j] = ac[abs(j-i)-1];
			}

		}

	}

	LAPACKE_dgesv(LAPACK_ROW_MAJOR, lag, 1, A, lag, ipiv, b, 1);

	double partial_correlation = b[lag-1];

	free(ac);
	free(A);
	free(b);
	free(ipiv);

	return partial_correlation;

}

void partial_autocorrelation_function(double* series,
                                      unsigned int size,
                                      unsigned int num_lags,
                                      double mean,
                                      double* pacf)
{

	unsigned int i;

	for(i=0;i<num_lags;i++) pacf[i] = partial_autocorrelation(series, size, i+1, mean);

}

void difference(double* series, unsigned int size, unsigned int d, double* differenced){

	double* temp = (double*)malloc(size*sizeof(double));
	memcpy(temp,series,size*sizeof(double));

	unsigned int i, j;
	for(i=0;i<d;i++){

		for(j=0;j<(size-i);j++) temp[j] -= temp[j+1];

	}

	memcpy(differenced,temp,(size-d)*sizeof(double));

	free(temp);

}

void undifference(double* series, unsigned int size, unsigned int d, double* pre_sample_series){

	unsigned int i, j;
	double* data;

	data = (double*)malloc((size+d)*sizeof(double));
	memcpy(data,series,size*sizeof(double));
	memcpy(data+size,pre_sample_series,d*sizeof(double));

	for(i=0;i<size;i++){
		int sign = 1;
		for(j=1;j<=d;j++){

			data[size-i-1] += sign*binomial(d,j)*data[size-i-j+1];
			sign *= -1;

		}

	}

	memcpy(series,data,size*sizeof(double));

	free(data);

}

void detrend(double* series, unsigned int size, double* alpha, double* beta, double* residuals){

	unsigned int i;

	double *A, *b;

	unsigned int rows, cols = 2;
	
	rows = size;

	A = (double*)malloc(rows*cols*sizeof(double));
	b = (double*)malloc(rows*sizeof(double));

	for(i=0;i<rows;i++){
		b[i] = series[i];

		A[cols*i] = rows - i;

		A[cols*i+1] = 1;
	}

	LAPACKE_dgels(LAPACK_ROW_MAJOR, 'N', rows, cols, 1, A, cols, b, 1);

	*beta = b[0];
	*alpha = b[1];

	for(i=0;i<size;i++){
	
		residuals[i] = series[i] - *alpha - (*beta * (size-i));

	}

}

unsigned int factorial(unsigned int n){

	unsigned int value = 1;

	while(n > 0){

		value *= n;
		n--;

	}

	return value;

}

unsigned int binomial(unsigned int n, unsigned int k){
	
	if(n < k) return 0;

	unsigned int value = factorial(n);
	value /= factorial(k);
	value /= factorial(n-k);

	return value;

}

double aic(unsigned int size, double rss, unsigned int k){

	return 2*k+size*log(rss);

}

double bic(unsigned int size, double rss, unsigned int k){

	return k*log(size)+size*log(rss/size);

}
