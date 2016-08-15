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

	double alpha;

	if( opt & ARIMAMODEL_FIT_TREND ){

		double* detrended = new double[size];

		detrend(series,size,&alpha,&beta,detrended);

		series = detrended;

	}else if(d > 0){ /* unlikely that it's both integrated and det. trend */

		double* differenced = new double[size-d];

		difference(series,size,d,differenced);

		series = differenced;

	}

	if(p > 0 && q == 0){ /*AR(p) model*/

		ar_yule_walker(series,size,&c,&(psi[0]),p,residuals);

		var = variance(residuals,size,mean(residuals,size));

	}else if(p >= 0 && q > 0){ /*ARMA(p,q) model*/

		arma_long_ar(series,size,&c,&(psi[0]),p,&(theta[0]),q,residuals);

		var = variance(residuals,size,mean(residuals,size));

	}

	unsigned int k = p + q + 1; /*ar, ma, & const paramters*/

	if( opt & ARIMAMODEL_FIT_TREND ) c = alpha; /*trend const. component*/

	AIC = aic(size-d,k,(size-d-1)*var); /*Akaike information criterion*/

	if(d > 0 || opt & ARIMAMODEL_FIT_TREND ) delete[] series;

	return 0;

}

ModelSpec ARIMAModel::getModelSpec(){

	ModelSpec specs;

	unsigned int i;
	specs.p = p;
	specs.d = d;
	specs.q = q;

	specs.psi.resize(p);
	for(i=0;i<p;i++) specs.psi[i] = psi[i];

	specs.theta.resize(q);
	for(i=0;i<q;i++) specs.theta[i] = theta[i];

	specs.c    = c;
	specs.beta = beta;
	specs.var  = var;

	return specs;

}

void ARIMAModel::setModelSpec(const ModelSpec & specs){

	unsigned int i;
	p = specs.p;
	d = specs.d;
	q = specs.q;

	psi.resize(p);
	for(i=0;i<p;i++) psi[i] = specs.psi[i];

	theta.resize(q);
	for(i=0;i<q;i++) theta[i] = specs.theta[i];

	c    = specs.c;
	beta = specs.beta;
	var  = specs.var;

}

int ARIMAModel::forecast(double* series, unsigned int size, double* innovations, int initial_time){

	unsigned int pre_sample_size = std::max(p,q);

	if(pre_sample_size > size)
	{
		std::cerr << "ERROR: forecast: sample size too small\n";
		return -1;
	}

	std::vector<double> pre_sample_series     (pre_sample_size);
	std::vector<double> pre_sample_innovations(pre_sample_size);

	/*
	 * First we must obtain the pre sampled
	 * stationary series before we can model
	 * the process
	 */
	 getStationaryProcess(pre_sample_series,
	 					  pre_sample_innovations,
	 					  series,innovations,size);


	/*
	 * Now for the simulation
	 */
	simulate(pre_sample_series,pre_sample_innovations,foreParams);

	addTrend(size+1,pre_sample_series);

	return 0;

}

void ARIMAModel::getStationaryProcess(std::vector<double> & pre_sample_series,
									  std::vector<double> & pre_sample_innovations,
									  double* series, double* innovations,
									  unsigned int size)
{

	unsigned int i;

	unsigned int sample_size = pre_sample_series.size();

	if(beta != 0){ /* subtract deterministic trend */

		for(i=0;i<sample_size;++i)
		{
			pre_sample_series[sample_size-i-1] = series[i] - c - beta*(size-i);
			if(innovations)
				pre_sample_innovations[sample_size-i-1] = innovations[i];
		}


	}else if(d > 0){ /* eliminate shocastic trend */

		/* THIS CODE NEEDS TO BE FIXED */
		double* data = new double[size-d];

		difference(series,size,d,data);

		for(i=0;i<sample_size;++i)
		{
			pre_sample_series[i] = data[sample_size-i-1];
			if(innovations)
				pre_sample_innovations[i] = innovations[sample_size-i-1];
		}

		delete[] data;

	}else{ /* do nothing */

		for(i=0;i<sample_size;++i)
		{
			pre_sample_series[sample_size-i-1] = series[i];

			if(innovations)
				pre_sample_innovations[sample_size-i-1] = innovations[i];
		}

	}

}

void ARIMAModel::simulate(std::vector<double> & pre_sample_series,
						  std::vector<double> & pre_sample_innovations,
						  struct ForecastParameters & foreParams)
{

	unsigned int i, j, k;

	std::default_random_engine generator;
	std::normal_distribution<double> noise(0.0f,sqrt(var));

	unsigned int projection = foreParams.projection;
	unsigned int trials     = foreParams.trials;

	forecasts.resize(trials,projection);

	for(i=0;i<trials;i++){

		for(j=0;j<projection;j++){
			double value = 0;

			for(k=0;k<p;k++)
				value +=   psi[k] * pre_sample_series[pre_sample_series.size()-k-1];

			for(k=0;k<q && k<pre_sample_innovations.size();k++)
				value += theta[k] * pre_sample_innovations[pre_sample_innovations.size()-k-1];

			double e = noise(generator);
			value += e;

			pre_sample_series.push_back(value);
			pre_sample_innovations.push_back(e);

			forecasts(i,j) = value;
		}

		for(j=0;j<projection;j++){
			pre_sample_series.pop_back();
			pre_sample_innovations.pop_back();
		}

	}
}

void ARIMAModel::addTrend(unsigned int start_time, std::vector<double> & pre_sample_series)
{

	unsigned int i, j;

	if(beta != 0) /* add deterministic trend */
	{
		for(i=0;i<foreParams.trials;++i)
			for(j=0;j<foreParams.projection;++j)
				forecasts(i,j) += c + beta * double(j + start_time);
	}
	else if(d > 0) /* re-integrate series */
	{
		for(i=0;i<forecasts.rows();++i)
		{
			std::vector<double> forecast = forecasts.row_slice(i,0,forecasts.cols());

			undifference(&(forecast[0]),foreParams.projection,d,&(pre_sample_series[0]));

			for(j=0;j<forecasts.cols();++j)
			{
				forecasts(i,j) = forecast[j];
			}
		}
	}

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
		upper[i] = mean + 1.96f*se;
		lower[i] = mean - 1.96f*se;

		delete[] sample;

	}

}

void ARIMAModel::setForecastParams(struct ForecastParameters* params){

	foreParams.projection = params->projection;
	foreParams.trials = params->trials;

}
