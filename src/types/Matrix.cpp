/********************************************************************
Matrix.cpp
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include "types/Matrix.hpp"
#include <sstream>
#include <stdexcept>
#include <cstddef>
#include <cstdlib>
#include <cstring>

extern "C"{

extern void dgemm_(char * transa, char * transb, int * m, int * n, int * k,
              double * alpha, double * A, int * lda,
              double * B, int * ldb, double * beta,
              double *, int * ldc);

}

Matrix::Matrix(){

	num_rows = 0;
	num_cols = 0;

	values = NULL;

}

Matrix::Matrix(unsigned int rows_in, unsigned int cols_in){

	num_rows = rows_in;
	num_cols = cols_in;

	if(rows_in > 0 && cols_in > 0)
		values = (double*)malloc(num_rows*num_cols*sizeof(double));
	else
		values = NULL;

}

Matrix::Matrix(const Matrix & rhs){

	num_rows = rhs.num_rows;
	num_cols = rhs.num_cols;

	if(rhs.values){

		values = (double*)malloc(num_rows*num_cols*sizeof(double));
		memcpy(values,rhs.values,num_rows*num_cols*sizeof(double));

	}else{

		values = NULL;

	}

}

Matrix::~Matrix(){

	if(values) free(values);
	values = NULL;

}

void Matrix::resize(unsigned int rows_in, unsigned int cols_in){

	if(values) free(values);

	num_rows = rows_in;
	num_cols = cols_in;

	if(rows_in > 0 && cols_in > 0) 
		values = (double*)malloc(num_rows*num_cols*sizeof(double));
	else
		values = NULL;

}

double Matrix::operator()(unsigned int row, unsigned int col) const{

	if(row >= num_rows || col >= num_cols){

		std::ostringstream error;
		error << row      << "," << col      << " is out of range of Matrix(";
		error << num_rows << "," << num_cols << ")\n";
		throw std::out_of_range(error.str());

	}

	return values[num_rows*col+row];

}

double& Matrix::operator()(unsigned int row, unsigned int col){

	if(row >= num_rows || col >= num_cols){

		std::ostringstream error;
		error << row      << "," << col      << " is out of range of Matrix(";
		error << num_rows << "," << num_cols << ")\n";
		throw std::out_of_range(error.str());

	}

	return values[num_rows*col+row];

}

std::string Matrix::to_string(){

	unsigned int i, j;
	std::ostringstream os;

	for(i=0;i<num_rows;i++){

		for(j=0;j<num_cols;j++){

			os << values[j*num_rows+i] << " ";

		}

		os << "\n";

	}

	return os.str();

}

Matrix Matrix::dot(Matrix & lhs, Matrix & rhs){
	
	Matrix c(lhs.rows(),rhs.cols());

	char trans_a = 'N';
	char trans_b = 'N';
	int m = lhs.rows();
	int n = rhs.cols();
	int k = lhs.cols();
	double alpha = 1;
	int lda = m;
	int ldb = k;
	double beta = 0;
	int ldc = m;

	dgemm_(&trans_a,&trans_b,&m,&n,&k,&alpha,lhs.values,&lda,rhs.values,&ldb,&beta,c.values,&ldc);

	return c;

}
