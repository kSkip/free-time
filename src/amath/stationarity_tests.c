/********************************************************************
stationarity_tests.c
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include "amath/statistics.h"
#include "amath/stationarity_tests.h"

#include <stdio.h>

/*
Dickey-Fuller critical t values
cols: sample size, 1% , %5
*/
const double df_table[6][3] = { {25, -2.66, -1.95},
				{50, -2.62, -1.95},
				{100, -2.60, -1.95},
				{250, -2.58, -1.95},
				{500, -2.58, -1.95},
				{100000, -2.58, -1.95} };

const double df_table_const[6][3] = { {25, -3.75, -3.00},
				      {50, -3.58, -2.93},
				      {100, -3.51, -2.89},
				      {250, -3.46, -2.88},
				      {500, -3.44, -2.87},
				      {100000, -3.43, -2.86} };

const double df_table_const_trend[6][3] = { {25, -4.38, -3.60},
					    {50, -4.15, -3.50},
					    {100, -4.04, -3.45},
					    {250, -3.99, -3.43},
					    {500, -3.98, -3.42},
					    {100000, -3.96, -3.41} };

/*
KPSS critical values
cols: 1%, %5
rows: const, const w/ trend
*/
const double kpss_table[2][3] = { {0.739, 0.463},
				  {0.216, 0.146} };

double df_table_value(unsigned int N, unsigned int col, unsigned int short opt);

unsigned int df(double* series_in, unsigned int size, unsigned short opt){

	int i, j;
	double* series = (double*)malloc(size*sizeof(double));
	double* dseries = (double*)malloc((size-1)*sizeof(double));
	double *A, *b;
	double delta;

	unsigned int rows = size - 2;
	unsigned int cols = 1;
	if(opt & DF_TREND) cols++;
	if(opt & DF_CONST) cols++;

	A = (double*)malloc(rows*cols*sizeof(double));
	b = (double*)malloc(rows*sizeof(double));

	memcpy(series,series_in,size*sizeof(double));

	difference(series,size,1,dseries);
	
	for(i=0;i<rows;i++){
		j = 0;
		A[cols*i] = series[i+1];
		b[i] = dseries[i];
		if(opt & DF_TREND){
			j++;
			A[cols*i+j] = rows - i;
		}
		if(opt & DF_CONST){
			j++;
			A[cols*i+j] = 1;
		}
	}

	LAPACKE_dgels(LAPACK_ROW_MAJOR, 'N', rows, cols, 1, A, cols, b, 1);

	delta = b[0];

	double rss = 0, sdelta, var;
	
	for(i=cols;i<rows;i++){
		rss += b[i]*b[i];
	}
	var = variance(&(series[1]),rows,mean(&(series[1]),rows));
	sdelta = sqrt( (rss/(rows-2)) / (rows*var) );

	double DF_t = delta / sdelta;

	double DF_table_value = df_table_value(rows,2,opt);

	if(opt & DF_VERBOSE){
		printf("gamma: %f\nSE[gamma]: %f\n",delta,sdelta);
		if(opt & DF_CONST && opt & DF_TREND){
			printf("beta: %f\n",b[1]);
			printf("c: %f\n",b[2]);
		}else if(opt & DF_CONST){
			printf("c: %f\n",b[1]);
		}else if(opt & DF_TREND){
			printf("beta: %f\n",b[1]);
		}
		printf("DF_t: %f\nDF_t_table: %f\n",DF_t,DF_table_value);
	}

	free(series);
	free(dseries);
	free(A);
	free(b);

	if(DF_t < DF_table_value) return 1;

	return 0;

}

unsigned int adf(double* series, unsigned int size){

	/*for augmented dickey-fuller test*/

	return 0;

}

double df_table_value(unsigned int N, unsigned int col, unsigned short opt){

	unsigned int i;
	double value;
	const double (*table)[3];

	if(opt & DF_CONST && opt & DF_TREND){
		table = df_table_const_trend;
	}else if(opt & DF_CONST){
		table = df_table_const;
	}else{
		table = df_table;
	}

	if( N <= table[0][0] ){
		value = table[0][col];
	}else if( N >= table[5][0] ){
		value = table[5][col];
	}else{
		for(i=0;i<5;i++){
			if( N >= table[i][0] && N <= table[i+1][0] ){
				if( (N - table[i][0]) <= ( table[i+1][0] - N) ){
					value = table[i][col];
				}else{
					value = table[i+1][col];
				}
			}
		}
	}

	return value;

}

unsigned int kpss(double* series_in, unsigned int size, unsigned int T, unsigned short opt){

	int i, j;
	double* series = (double*)malloc(size*sizeof(double));
	double* residuals = (double*)malloc(size*sizeof(double));
	double* S_t = (double*)malloc(size*sizeof(double));
	double *A, *b;
	double c, beta;
	double KPSS_T;

	unsigned int rows = size;
	unsigned int cols = 1;
	if(opt & KPSS_TREND) cols++;

	A = (double*)malloc(rows*cols*sizeof(double));
	b = (double*)malloc(rows*sizeof(double));

	memcpy(series,series_in,size*sizeof(double));

	for(i=0;i<rows;i++){
		A[cols*i] = series[i+1];
		b[i] = series[i];
		if(opt & KPSS_TREND){
			A[cols*i+1] = rows - i;
		}
	}

	LAPACKE_dgels(LAPACK_ROW_MAJOR, 'N', rows, cols, 1, A, cols, b, 1);

	c = b[0];
	if(opt & KPSS_TREND) beta = b[1];

	for(i=0;i<size;i++){
		residuals[i] = series[i]-c-beta*(size-i);
	}

	for(i=0;i<size;i++){
		S_t[i] = 0;
		for(j=0;j<=i;j++){
			S_t[i] += residuals[size-i-1];
		}
	}
	double var_r = variance(residuals,size,0);

	KPSS_T = 0;
	for(i=0;i<size;i++){
		KPSS_T += S_t[i]*S_t[i];
	}
	KPSS_T /= size*size;

	double omega_T = var_r;

	for(i=1;i<=T;i++){
		double gamma_tau = 0;
		for(j=i+1;j<=size;j++){
			gamma_tau += residuals[size-j]*residuals[(size-j+i)%size];
		}
		omega_T += 2*( 1 - ((i/(T-1))*(gamma_tau/size)) );
	}
	KPSS_T /= omega_T;

	double KPSS_critical;
	if(opt & KPSS_TREND) KPSS_critical = kpss_table[1][1];
	else KPSS_critical = kpss_table[0][1]; 

	if(opt & KPSS_VERBOSE){
		printf("c: %f\n",c);
		if(opt & KPSS_TREND) printf("beta: %f\n",beta);
		printf("KPSS: %f\n",KPSS_T);
		printf("KPSS critical: %f\n",KPSS_critical);
	}

	free(series);
	free(residuals);
	free(S_t);
	free(A);
	free(b);

	if(KPSS_T > KPSS_critical) return 1;

	return 0;

}
