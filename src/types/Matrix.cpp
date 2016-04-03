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

#ifdef CUDA
#include "cuda/MatrixOp.h"
#endif

extern "C"{

extern void dgemm_(char * transa, char * transb, int * m, int * n,
              int * k, double * alpha, double * A, int * lda,
              double * B, int * ldb, double * beta,
              double *, int * ldc);

extern void dgesv_(int * n, int * nrhs, double * A, int * lda,
              int * ipiv, double * B, int * ldb, int * info);

extern void dgels_(char * trans, int * m, int * n, int * nrhs,
              double * A, int * lda, double * b, int * ldb,
              double * work, int * lwork, int * info);

}

Matrix::Matrix(){

    /*
     * set defaults: values zero, pointers null
     */

	num_rows = 0;
	num_cols = 0;

	values = NULL;
    #ifdef CUDA
    pDev = NULL;
    #endif

    #ifdef CUDA
    cublasCreate(&hCudaBlas);
    #endif

}

Matrix::Matrix(unsigned int rows_in, unsigned int cols_in){

	num_rows = rows_in;
	num_cols = cols_in;

    /*
     * only allocate memory when the size is non-zero
     */

	if(rows_in > 0 && cols_in > 0)
    {
		values = (double*)malloc(num_rows*num_cols*sizeof(double));
        #ifdef CUDA
        cudaMalloc((void**)&pDev,num_rows*num_cols*sizeof(double));
        #endif
    }
	else
    {
		values = NULL;
        #ifdef CUDA
        pDev = NULL;
        #endif
    }

    #ifdef CUDA
    cublasCreate(&hCudaBlas);
    #endif

}

Matrix::Matrix(unsigned int rows_in, unsigned int cols_in, double value){

	num_rows = rows_in;
	num_cols = cols_in;

    /*
     * only allocate memory when the size is non-zero
     */

	if(rows_in > 0 && cols_in > 0)
    {
		values = (double*)malloc(num_rows*num_cols*sizeof(double));
        #ifdef CUDA
        cudaMalloc((void**)&pDev,num_rows*num_cols*sizeof(double));
        #endif
    }
	else
    {
		values = NULL;
        #ifdef CUDA
        pDev = NULL;
        #endif
    }

    /*
     * assign all elements to given value
     */

	unsigned int i;
	for(i=0;i<num_rows*num_cols;++i){
		values[i] = value;
	}

    #ifdef CUDA
    cublasCreate(&hCudaBlas);
    #endif

}

Matrix::Matrix(const Matrix & rhs){

	num_rows = rhs.num_rows;
	num_cols = rhs.num_cols;

    /*
     * copy the values if the data is there
     */

	if(rhs.values)
    {

		values = (double*)malloc(num_rows*num_cols*sizeof(double));
		memcpy(values,rhs.values,num_rows*num_cols*sizeof(double));
        #ifdef CUDA
        cudaMalloc((void**)&pDev,num_rows*num_cols*sizeof(double));
        #endif

	}
    else{

		values = NULL;
        #ifdef CUDA
        pDev = NULL;
        #endif
	}

    #ifdef CUDA
    cublasCreate(&hCudaBlas);
    #endif

}

Matrix::~Matrix(){

    /*
     * free everything
     */

	if(values) free(values);
	values = NULL;

    #ifdef CUDA
    if(pDev) cudaFree(pDev);
    pDev = NULL;

	cublasDestroy(hCudaBlas);
    #endif

}

void Matrix::resize(unsigned int rows_in, unsigned int cols_in){

	if(values) free(values);
    #ifdef CUDA
    if(pDev) cudaFree(pDev);
    #endif

	num_rows = rows_in;
	num_cols = cols_in;

	if(rows_in > 0 && cols_in > 0)
    {
		values = (double*)malloc(num_rows*num_cols*sizeof(double));
        #ifdef CUDA
        cudaMalloc((void**)&pDev,num_rows*num_cols*sizeof(double));
        #endif
    }
	else
    {
		values = NULL;
        #ifdef CUDA
        pDev = NULL;
        #endif
    }

}

double Matrix::operator()(unsigned int row, unsigned int col) const{

    /*
     * check bounds
     */

	if(row >= num_rows || col >= num_cols){

		std::ostringstream error;
		error << row      << "," << col      << " is out of range of Matrix(";
		error << num_rows << "," << num_cols << ")";
		throw std::out_of_range(error.str());

	}

	return values[num_rows*col+row];

}

double& Matrix::operator()(unsigned int row, unsigned int col){

    /*
     * check bounds
     */

	if(row >= num_rows || col >= num_cols){

		std::ostringstream error;
		error << row      << "," << col      << " is out of range of Matrix(";
		error << num_rows << "," << num_cols << ")";
		throw std::out_of_range(error.str());

	}

	return values[num_rows*col+row];

}

Matrix& Matrix::operator=(const Matrix & rhs)
{

    num_rows = rhs.num_rows;
	num_cols = rhs.num_cols;

	if(rhs.values){

		values = (double*)malloc(num_rows*num_cols*sizeof(double));
		memcpy(values,rhs.values,num_rows*num_cols*sizeof(double));
        #ifdef CUDA
        cudaMalloc((void**)&pDev,num_rows*num_cols*sizeof(double));
        #endif

	}else{

		values = NULL;
        #ifdef CUDA
        pDev = NULL;
        #endif

	}

}

std::vector<double> Matrix::row_slice(unsigned int row_num, unsigned int first, unsigned int second){

	if(row_num >= num_rows || first >= num_cols || second >= num_cols || first > second){

		std::ostringstream error;
		error << "row_slice: invalid slice (";
		error << row_num << "," << first << "," << second << ")";
		throw std::out_of_range(error.str());

	}

	std::vector<double> row;

	row.resize(second+1-first);

	unsigned int i;
	for(i=0;i<row.size();++i){

		row[i] = *(values + row_num + num_cols*i);

	}

	return row;

}

std::vector<double> Matrix::col_slice(unsigned int col_num, unsigned int first, unsigned int second){

	if(col_num >= num_cols || first >= num_rows || second >= num_rows || first > second){

		std::ostringstream error;
		error << "col_slice: invalid slice (";
		error << col_num << "," << first << "," << second << ")";
		throw std::out_of_range(error.str());

	}

	std::vector<double> col;

	col.assign(values + col_num*num_rows + first,values + col_num*num_rows + second+1);

	return col;

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

	if(lhs.num_cols != rhs.num_rows){

		std::ostringstream error;
		error << "dot: LHS cols not equal to RHS rows";
		throw std::length_error(error.str());

	}
	
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

    /*
     * cublas and blas function arguments
     * have nice one-to-one correspondence
     */

    #ifdef CUDA

    cublasSetMatrix(m,k,sizeof(double),lhs.values,lda,lhs.pDev,lda);
    cublasSetMatrix(k,n,sizeof(double),rhs.values,ldb,rhs.pDev,ldb);

    cublasDgemm(lhs.hCudaBlas,CUBLAS_OP_N,CUBLAS_OP_N,
                m,n,k,
                &alpha,
                lhs.pDev, lda,
                rhs.pDev, ldb,
                &beta,
                c.pDev, ldc);

    cublasGetMatrix(m,n,sizeof(double),c.pDev,ldc,c.values,ldc);

    #else

	dgemm_(&trans_a,&trans_b,&m,&n,&k,&alpha,
           lhs.values,&lda,rhs.values,&ldb,&beta,
           c.values,&ldc);

    #endif

	return c;

}

Matrix Matrix::had(Matrix & lhs, Matrix & rhs)
{

    if(lhs.rows() != rhs.rows() || lhs.cols() != rhs.cols())
        throw std::runtime_error("Matrix dimensions must be the same for Hadamard product");

    Matrix c(lhs.rows(),lhs.cols());

    /*
     * for using the GPU we need to copy to
     * device memory, call kernel, then copy result
     */

    #ifdef CUDA

    cudaMemcpy(lhs.pDev,lhs.values,
               lhs.rows()*lhs.cols()*sizeof(double),
               cudaMemcpyHostToDevice);

    cudaMemcpy(rhs.pDev,rhs.values,
               rhs.rows()*rhs.cols()*sizeof(double),
               cudaMemcpyHostToDevice);

    dhad(lhs.pDev,rhs.pDev,c.pDev,lhs.rows()*lhs.cols());

    cudaMemcpy(c.values,c.pDev,
               c.rows()*c.cols()*sizeof(double),
               cudaMemcpyDeviceToHost);

    #else

    unsigned int i;
    for(i=0;i<lhs.rows()*lhs.cols();++i)
    {
        c.values[i] = lhs.values[i] * rhs.values[i];
    }

    #endif

    return c;

}

Matrix Matrix::add(Matrix & lhs, Matrix & rhs){

	if(lhs.num_rows != rhs.num_rows || lhs.num_cols != rhs.num_cols){

		std::ostringstream error;
		error << "add: Matrix dimensions do not match";
		throw std::length_error(error.str());

	}

	Matrix c(lhs.num_rows,lhs.num_cols);

    #ifdef CUDA

    cudaMemcpy(lhs.pDev,lhs.values,
               lhs.rows()*lhs.cols()*sizeof(double),
               cudaMemcpyHostToDevice);

    cudaMemcpy(rhs.pDev,rhs.values,
               rhs.rows()*rhs.cols()*sizeof(double),
               cudaMemcpyHostToDevice);

    dadd(lhs.pDev,rhs.pDev,c.pDev,lhs.rows()*lhs.cols());

    cudaMemcpy(c.values,c.pDev,
               c.rows()*c.cols()*sizeof(double),
               cudaMemcpyDeviceToHost);

    #else

	unsigned int i;

	for(i=0;i<lhs.num_rows*lhs.num_cols;++i){

		c.values[i] = lhs.values[i] + rhs.values[i];

	}

    #endif

	return c;

}

Matrix Matrix::subtract(Matrix & lhs, Matrix & rhs){

	if(lhs.num_rows != rhs.num_rows || lhs.num_cols != rhs.num_cols){

		std::ostringstream error;
		error << "subtract: Matrix dimensions do not match";
		throw std::length_error(error.str());

	}

	Matrix c(lhs.num_rows,lhs.num_cols);

    #ifdef CUDA

    cudaMemcpy(lhs.pDev,lhs.values,
               lhs.rows()*lhs.cols()*sizeof(double),
               cudaMemcpyHostToDevice);

    cudaMemcpy(rhs.pDev,rhs.values,
               rhs.rows()*rhs.cols()*sizeof(double),
               cudaMemcpyHostToDevice);

    dsub(lhs.pDev,rhs.pDev,c.pDev,lhs.rows()*lhs.cols());

    cudaMemcpy(c.values,c.pDev,
               c.rows()*c.cols()*sizeof(double),
               cudaMemcpyDeviceToHost);

    #else

	unsigned int i;

	for(i=0;i<lhs.num_rows*lhs.num_cols;++i){

		c.values[i] = lhs.values[i] - rhs.values[i];

	}

    #endif

	return c;

}

Matrix Matrix::T()
{

    Matrix trans(num_cols,num_rows);

    unsigned int i, j;
    for(i=0;i<num_rows;++i)
    {
        for(j=0;j<num_cols;++j)
        {
            trans(j,i) = (*this)(i,j);
        }
    }

    return trans;

}

/*
 * all the various definitions for the
 * overloaded operators
 */

Matrix Matrix::operator*(Matrix & rhs){

	return dot(*this,rhs);

}

Matrix Matrix::operator*(Matrix rhs){

	return dot(*this,rhs);

}

Matrix Matrix::operator+(Matrix & rhs){

	return add(*this,rhs);

}

Matrix Matrix::operator+(Matrix rhs){

	return add(*this,rhs);

}

Matrix Matrix::operator-(Matrix & rhs){

	return subtract(*this,rhs);

}

Matrix Matrix::operator-(Matrix rhs){

	return subtract(*this,rhs);

}

Matrix Matrix::apply(double (*func)(double))
{

    Matrix result(num_rows,num_cols);

    unsigned int i;
    for(i=0;i<num_rows*num_cols;++i)
    {
        result.values[i] = func(values[i]);
    }

    return result;

}

Matrix Matrix::apply(double (*func)(double,double), const Matrix& mat1, const Matrix& mat2)
{

    if(mat1.rows() != mat2.rows() || mat1.cols() != mat2.cols())
    {
        throw std::runtime_error("\"apply\" must be used with matrices of same size");
    }

    unsigned int i;
    Matrix result(mat1.rows(),mat1.cols());
    for(i=0;i<mat1.rows()*mat1.cols();++i)
    {
        result.values[i] = func(mat1.values[i],mat2.values[i]);
    }

    return result;


}

Matrix Matrix::linear_solve(Matrix & A, Matrix & b){

	Matrix sol = b;

	char trans = 'N';
	int m = A.rows();
	int n = A.cols();
	int nrhs = b.cols();
	int lda = m;
	int ldb = m;
	int lwork = -1;
	double* work = (double*)malloc(sizeof(double));
	int info;

	if(A.num_rows == A.num_cols){
	
		int* ipiv = (int*)malloc(A.num_cols*sizeof(int));
		dgesv_(&n,&nrhs,A.values,&n,ipiv,sol.values,&n,&info);
		free(ipiv);

	}else{

		dgels_(&trans,&m,&n,&nrhs,A.values,&lda,sol.values,&ldb,work,&lwork,&info);

		lwork = work[0];
		free(work);
		work = (double*)malloc(lwork*sizeof(double));

		dgels_(&trans,&m,&n,&nrhs,A.values,&lda,sol.values,&ldb,work,&lwork,&info);

	}

	return sol;

}
