/********************************************************************
ARIMAModel.hpp
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#ifndef ARIMAMODEL_HPP
#define ARIMAMODEL_HPP

#include <vector>
#include "types/Matrix.hpp"

#define ARIMAMODEL_FIT_CONST 0
#define ARIMAMODEL_FIT_TREND 1

struct ForecastParameters{
	unsigned int projection;
	unsigned int trials;
};

class ModelSpec{
	public:
		ModelSpec();

		unsigned int p, d, q;
		std::vector<double> psi;
		std::vector<double> theta;
		double c;
		double beta;
		double var;
		
		void print();
		
		int save(const char* filename);
		int load(const char* filename);
};

class ARIMAModel{
	public:
		ARIMAModel();
		ARIMAModel(unsigned int p, unsigned int d, unsigned int q);
		ARIMAModel(const ARIMAModel & rhs);
		~ARIMAModel();

		int fit(double* series, unsigned int size, double* residuals, unsigned short opt);
		void getModelSpec(ModelSpec* specs);
		void setModelSpec(ModelSpec* specs);
		double getAIC(){ return AIC; }

		int forecast(double* series, unsigned int size, double* innovations, int initial_time);
		void getForecast(double* expectation, double* upper, double* lower);
		void setForecastParams(struct ForecastParameters* params);
	protected:
		unsigned int p, d, q;
		std::vector<double> psi;
		std::vector<double> theta;
		double c;
		double beta;
		double var;

		double AIC;

		Matrix forecasts;
		struct ForecastParameters foreParams;
};

#endif
