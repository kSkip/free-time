/********************************************************************
ARIMAModel.cpp
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include "regression/ARIMAModel.hpp"

#include <iostream>
#include <algorithm>
#include <random>
#include <cmath>

#include "amath/amath.h"

//default constructor
ARIMAModel::ARIMAModel(){

	p = 0;
	q = 0;
	d = 0;
	foreParams.projection = 0;
	foreParams.trials = 0;

	c = 0;
	beta = 0;
	var = 0;

}

//overloaded constructor
ARIMAModel::ARIMAModel(unsigned int p_in, unsigned int d_in, unsigned int q_in){

	p = p_in;
	psi.resize(p);

	q = q_in;
	theta.resize(q);

	d = d_in;

	foreParams.projection = 0;
	foreParams.trials = 0;

	c = 0;
	beta = 0;
	var = 0;

}

//default copy constructor
ARIMAModel::ARIMAModel(const ARIMAModel & rhs){

	p = rhs.p;
	psi.assign(rhs.psi.begin(),rhs.psi.end());

	q = rhs.q;
	theta.assign(rhs.theta.begin(),rhs.theta.end());

	d = rhs.d;

	forecasts = rhs.forecasts;

	foreParams.projection = rhs.foreParams.projection;
	foreParams.trials = rhs.foreParams.trials;

	c = rhs.c;
	beta = rhs.beta;
	var = rhs.var;

}

//default destructor
ARIMAModel::~ARIMAModel(){
	
	psi.clear();
	
	theta.clear();

}

int ARIMAModel::fit(double* series, unsigned int size, double* residuals, unsigned short opt){

	if(p > 0 && d == 0 && q == 0){ /*AR(p) model*/

		ar_yule_walker(series,size,&c,&(psi[0]),p,residuals);

		var = variance(residuals,size,mean(residuals,size));

	}else if(p >= 0 && d == 0 && q > 0){ /*ARMA(p,q) model*/

		arma_long_ar(series,size,&c,&(psi[0]),p,&(theta[0]),q,residuals);

		var = variance(residuals,size,mean(residuals,size));

	}else if(p > 0 && d > 0 && q == 0){ /*ARIMA(p,d,0) model*/
		
		double* differenced = new double[size-d];

		difference(series,size,d,differenced);

		ar_yule_walker(differenced,size-d,&c,&(psi[0]),p,residuals);

		var = variance(residuals,size-d,0);

		delete[] differenced;

	}else if(p >= 0 && d > 0 && q > 0){ /*ARIMA(p,d,q) model*/
		
		double* differenced = new double[size-d];

		difference(series,size,d,differenced);

		arma_long_ar(differenced,size-d,&c,&(psi[0]),p,&(theta[0]),q,residuals);

		var = variance(residuals,size-d,0);

		delete[] differenced;

	}

	unsigned int k = p + q + 1; /*ar, ma, & const paramters*/
	if( opt & ARIMAMODEL_FIT_TREND ) k++; /*trend component*/

	AIC = aic(size-d,k,(size-d-1)*var); /*Akaike information criterion*/

	return 0;

}

void ARIMAModel::getModelSpec(struct ModelSpec* specs){

	unsigned int i;
	specs->p = p;
	specs->d = d;
	specs->q = q;

	specs->psi.resize(p);
	for(i=0;i<p;i++) specs->psi[i] = psi[i];

	specs->theta.resize(q);
	for(i=0;i<q;i++) specs->theta[i] = theta[i];

	specs->c = c;
	specs->beta = beta;
	specs->var = var;

}

void ARIMAModel::setModelSpec(ModelSpec* specs){

	unsigned int i;
	p = specs->p;
	d = specs->d;
	q = specs->q;

	psi.resize(p);
	for(i=0;i<p;i++) psi[i] = specs->psi[i];

	theta.resize(q);
	for(i=0;i<q;i++) theta[i] = specs->theta[i];

	c = specs->c;
	beta = specs->beta;
	var = specs->var;

}

int ARIMAModel::forecast(double* series, unsigned int size, double* innovations, int initial_time){

	std::vector<double> pre_sample_series;
	std::vector<double> pre_sample_innovations;

	unsigned int pre_sample_size = std::max(p,q);
	if(pre_sample_size > size){
		std::cerr << "ERROR: forecast: sample size too small\n";
		return -1;
	}

	std::default_random_engine generator;
	std::normal_distribution<double> noise(0.0f,sqrt(var));
	unsigned int projection = foreParams.projection;
	unsigned int trials = foreParams.trials;

	forecasts.resize(trials,projection);

	unsigned int i, j, k;
	double* data;

	if(d > 0){
		data = new double[size-d];
		difference(series,size,d,data);

		for(i=0;i<pre_sample_size;i++){
			pre_sample_series.push_back(data[pre_sample_size-i-1]);
			if(innovations) pre_sample_innovations.push_back(innovations[pre_sample_size-i-1]);
		}
		
		delete[] data;
	}else{
		for(i=0;i<pre_sample_size;i++){
			pre_sample_series.push_back(series[pre_sample_size-i-1]);
			if(innovations) pre_sample_innovations.push_back(innovations[pre_sample_size-i-1]);
		}
	}

	for(i=0;i<trials;i++){

		for(j=0;j<projection;j++){
			double value = 0;

			for(k=0;k<p;k++){
				value += psi[k]*pre_sample_series[pre_sample_series.size()-k-1];
			}
		
			for(k=0;k<q && k<pre_sample_innovations.size();k++){
				value += theta[k]*pre_sample_innovations[pre_sample_innovations.size()-k-1];
			}
			double e = noise(generator);
			value += c + beta*(initial_time + j + 1) + e;
	
			pre_sample_series.push_back(value);
			pre_sample_innovations.push_back(e);

			forecasts(i,j) = value;
		}
		for(j=0;j<projection;j++){
			pre_sample_series.pop_back();
			pre_sample_innovations.pop_back();
		}

		//if(d > 0) undifference(forecasts[i],projection,d,series);

	}
	
	return 0;
	
}

void ARIMAModel::getForecast(double* expectation, double* upper, double* lower){

	unsigned int i, j;

	for(i=0;i<foreParams.projection;i++){

		double mean = 0, se;
		double* sample = new double[foreParams.trials];

		for(j=0;j<foreParams.trials;j++){
			mean += forecasts(j,i);
			sample[j] = forecasts(j,i);
		}
		mean /= foreParams.trials;
		se = sqrt(variance(sample,foreParams.trials,mean));

		expectation[i] = mean;
		upper[i] = mean + 1.96*se;
		lower[i] = mean - 1.96*se;

		delete[] sample;
		
	}

}

void ARIMAModel::setForecastParams(struct ForecastParameters* params){

	foreParams.projection = params->projection;
	foreParams.trials = params->trials;

}
