/********************************************************************
Matrix.hpp
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <string>
#include <vector>

#ifdef CUDA
#include <cuda_runtime.h>
#include <cublas_v2.h>
#endif

class Matrix{

	public:

		Matrix();
		Matrix(unsigned int num_rows, unsigned int num_cols);
		Matrix(unsigned int num_rows, unsigned int num_cols, double value);
		Matrix(const Matrix & rhs);
		~Matrix();

		void resize(unsigned int num_rows, unsigned int num_cols);

		unsigned int rows() const { return num_rows; }

		unsigned int cols() const { return num_cols; }

		double operator()(unsigned int row, unsigned int col) const;
		double& operator()(unsigned int row, unsigned int col);

        Matrix& operator=(const Matrix & rhs);

		std::vector<double> row_slice(unsigned row, unsigned int first, unsigned int second);
		std::vector<double> col_slice(unsigned row, unsigned int first, unsigned int second);

		std::string to_string();

		static Matrix dot(const Matrix & lhs, const Matrix & rhs);
        static Matrix had(const Matrix & lhs, const Matrix & rhs);
		static Matrix add(const Matrix & lhs, const Matrix & rhs);
		static Matrix subtract(const Matrix & lhs, const Matrix & rhs);

		Matrix operator*(const Matrix & rhs);
		Matrix operator+(const Matrix & rhs);
		Matrix operator-(const Matrix & rhs);

		friend Matrix operator*(double scalar, const Matrix& rhs);

        Matrix T(); /* matrix transpose */

        Matrix apply(double (*func)(double));

        static Matrix apply(double (*func)(double,double), const Matrix& mat1, const Matrix& mat2);

		static Matrix linear_solve(Matrix & A, Matrix & b);

	private:

		double* values;

		unsigned int num_rows, num_cols;

        #ifdef CUDA
        cublasHandle_t hCudaBlas;

        double* pDev;
        #endif

};

Matrix operator*(double scalar, const Matrix& rhs);

#endif
