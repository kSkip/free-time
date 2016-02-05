C= gcc
CC= g++
AR=ar
CFLAGS=-c -Wall
ARFLAGS=rs

INCLUDES= -I../../include -I/usr/local/include/lapacke -I../../third-party
LIBDIRS= -L/usr/local/lib -L../../lib
LIBS_LAPACK= -llapacke -llapack -lrefblas -lgfortran
