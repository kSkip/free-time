#include "cuda/MatrixOp.h"

/*
 * kernel function to compute Hadamard product
 */
__global__ void double_had(double* a, double* b, double* c)
{

	c[blockIdx.x] = a[blockIdx.x] * b[blockIdx.x];

}

void dhad(double* a, double* b, double* c, size_t dim)
{

    double_had<<<dim,1>>>(a,b,c);

}

/*
 * kernel function for addition
 */
__global__ void double_add(double* a, double* b, double* c)
{

	c[blockIdx.x] = a[blockIdx.x] + b[blockIdx.x];

}

void dadd(double* a, double* b, double* c, size_t dim)
{

    double_add<<<dim,1>>>(a,b,c);

}

/*
 * kernel function for subtraction
 */
__global__ void double_subtract(double* a, double* b, double* c)
{

	c[blockIdx.x] = a[blockIdx.x] - b[blockIdx.x];

}

void dsub(double* a, double* b, double* c, size_t dim)
{

    double_subtract<<<dim,1>>>(a,b,c);

}
