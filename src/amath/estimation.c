/********************************************************************
estimation.c
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include "amath/estimation.h"
#include "amath/statistics.h"
#include "amath/min_dist.h"

#include <stdio.h>

void ar_yule_walker(double* series, unsigned int size, double* c, double* psi, unsigned int p, double* residuals){

	int i, j;
	
	double series_mean = mean(series,size);
	double *A, *b;

	int ipiv[p];

	if(p<1) return;

	A = (double*)malloc(p*p*sizeof(double));
	b = (double*)malloc(p*sizeof(double));

	/*calculate all the autocorrelations*/
	for(i=0;i<(int)p;i++) b[i] = autocorrelation(series,size,i+1,series_mean);

	/*construct the yule-walker equations*/
	for(i=0;i<(int)p;i++){

		for(j=0;j<(int)p;j++){

			if(i == j) A[p*i+j] = 1.0f;
			else       A[p*i+j] = b[abs(j-i)-1];

		}

	}

	/*call the linear equations solvers*/
	LAPACKE_dgesv(LAPACK_ROW_MAJOR, p, 1, A, p, ipiv, b, 1);

	/*obtain the model coefficients*/
	double factor = 1;
	for(i=0;i<(int)p;i++){
		psi[i] = b[i];
		factor -= psi[i];
	}
	*c = series_mean*factor;

	for(i=0;i<(int)size;i++){

		residuals[i] = series[i] - *c;

		/*using cyclic boundaries here*/
		for(j=0;j<(int)p;j++) residuals[i] -= psi[j]*series[(i+j+1)%size];

	}

	free(A);
	free(b);

}

void ar_ols(double* series, unsigned int size, double* c, double* psi, unsigned int p, double* residuals){

	unsigned int i, j;
	
	double series_mean = mean(series,size);
	double *A, *b;

	if(p<1) return;

	unsigned int rows, cols;

	rows = size - p;
	cols = p;

	A = (double*)malloc(rows*cols*sizeof(double));
	b = (double*)malloc(rows*sizeof(double));

	/*construct the linear system*/
	for(i=0;i<rows;i++){

		b[i] = series[i] - series_mean;

		for(j=0;j<cols;j++) A[cols*i+j] = series[i+j+1] - series_mean;

	}

	/*call the least squares solver*/
	LAPACKE_dgels(LAPACK_ROW_MAJOR, 'N', rows, cols, 1, A, cols, b, 1);

	/*obtain the model coefficients*/
	double factor = 1;
	for(i=0;i<p;i++){
		psi[i] = b[i];
		factor -= psi[i];
	}
	*c = series_mean*factor;

	for(i=0;i<size;i++){

		residuals[i] = series[i] - *c;

		/*using cyclic boundaries here*/
		for(j=0;j<p;j++) residuals[i] -= psi[j]*series[(i+j+1)%size];

	}

	free(A);
	free(b);

}

void arma_long_ar(double* series,
                  unsigned int size,
                  double* constant,
                  double* psi,
                  unsigned int p,
                  double* theta,
                  unsigned int q,
                  double* residuals)
{

	unsigned int i, j;

	double series_mean = mean(series,size);
	double *A, *b;

	if(p > size || q > size) return;

	double* innovations = (double*)malloc(size*sizeof(double));
	
	unsigned int p_large;
	if(size > 180) p_large = 90;
	else p_large = (unsigned int)floor((double)size/2.0f);

	/*fit a long ar model to the series*/
	double* long_ar_psi = (double*)malloc(p_large*sizeof(double));
	double c;
	ar_ols(series,size,&c,long_ar_psi,p_large,innovations);
	free(long_ar_psi);
	/*"innovations" contains the sample we will use for the error terms*/

	unsigned int rows;
	if(p > q) rows = size - p;
	else      rows = size - q;

	unsigned int cols = p + q;

	A = (double*)malloc(rows*cols*sizeof(double));
	b = (double*)malloc(rows*sizeof(double));

	/*construct the linear system*/
	for(i=0;i<rows;i++){

		b[i] = series[i] - series_mean;

		for(j=0;j<p;j++) A[cols*i+j] = series[i+j+1] - series_mean;

		for(j=0;j<q;j++) A[cols*i+j+p] = innovations[i+j+1];

	}

	/*call the least squares solver*/
	LAPACKE_dgels(LAPACK_ROW_MAJOR, 'N', rows, cols, 1, A, cols, b, 1);

	/*obtain the model coefficients*/
	double factor = 1;
	for(i=0;i<p;i++){
		psi[i] = b[i];
		factor -= psi[i];
	}
	*constant = series_mean*factor;

	for(i=0;i<q;i++) theta[i] = b[i+p];

	memcpy(residuals,series,size*sizeof(double));

	for(i=0;i<size;i++){

		for(j=0;j<p;j++) residuals[i] -= psi[j]*series[(i+j+1)%size];

		for(j=0;j<q;j++) residuals[i] -= theta[j]*innovations[(i+j+1)%size];

		residuals[i] -= *constant;

	}

	free(A);
	free(b);
	free(innovations);

}

void var_ols(double** series, unsigned int size_time, unsigned int size_var, double* c, double** A, double* residuals){

}


void est_step_function(double* data, unsigned int size, unsigned int steps, double** fit, double* e_out){

	unsigned int i, j;
	double e;

	e = min_dist(data,size,steps);

	double* step_func = (double*)malloc(4*steps*sizeof(double));
	double min, max, step;
	min = data[0];
	max = data[0];
	step = data[0];
	step_func[0] = 0;
	j = 0;
	for(i=1;i<size;i++){
		if(data[i] > max){
			if( (data[i] - min)/2.0 > e){
				step_func[2*j+1] = step;
				step_func[2*(j+1)] = i;
				step_func[2*(j+1)+1] = step;
				step_func[2*(j+2)] = i;
				j+=2;
				max = data[i];
				min = data[i];
				step = data[i];
			}else{
				max = data[i];
				step = (max + min)/2.0;
			}
		}else if(data[i] < min){
			if( (max - data[i])/2.0 > e){
				step_func[2*j+1] = step;
				step_func[2*(j+1)] = i;
				step_func[2*(j+1)+1] = step;
				step_func[2*(j+2)] = i;
				j+=2;
				max = data[i];
				min = data[i];
				step = data[i];
			}else{
				min = data[i];
				step = (max + min)/2.0;
			}
		}
	}
	step_func[2*j+1] = step;
	step_func[2*(j+1)] = i-1;
	step_func[2*(j+1)+1] = step;

	*fit = step_func;
	*e_out = e;

}


void infer_lags(double* series, unsigned int size, unsigned int max_lags, double* p, double* q){

	double* acf;
	double* pacf;
	double* step_func;
	double series_mean, e;
	double step_size;

	*p = 0;
	*q = 0;

	acf = (double*)malloc(max_lags*sizeof(double));
	pacf = (double*)malloc(max_lags*sizeof(double));

	series_mean = mean(series,size);

	autocorrelation_function(series,size,max_lags,series_mean,acf);

	partial_autocorrelation_function(series,size,max_lags,series_mean,pacf);


	est_step_function(acf,max_lags,2,&step_func,&e);
	step_size = step_func[3] - step_func[5];
	if( e / step_size > 0.25 ){
		*q = 0;
	}else{
		*q = (unsigned int)step_func[2];
	}
	
	free(step_func);
	est_step_function(pacf,max_lags,2,&step_func,&e);
	step_size = step_func[3] - step_func[5];
	if( e / step_size > 0.25 ){
		*p = 0;
	}else{
		*p = (unsigned int)step_func[2];
	}

	

	if(*q == 0 && *p == 0){
		*q = step_func[2];
		*p = step_func[2];
	}

	free(acf);
	free(pacf);
	free(step_func);

}
