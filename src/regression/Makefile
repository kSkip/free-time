include ../../make.inc

all:
	$(CC) $(CFLAGS) -std=c++0x $(INCLUDES) ARIMAModel.cpp
	$(CC) $(CFLAGS) $(INCLUDES) ModelSpec.cpp
	$(CC) $(CFLAGS) $(INCLUDES) arimafit.cpp
	$(CC) $(CFLAGS) $(INCLUDES) arimaforecast.cpp

exe:
	$(CC) $(INCLUDES) $(LIBDIRS) ./arimafit.o ./ARIMAModel.o ./ModelSpec.o ../types/Matrix.o -o ../../bin/arimafit -lamath $(LIBS_LAPACK)
	$(CC) $(INCLUDES) $(LIBDIRS) ./arimaforecast.o ./ARIMAModel.o ./ModelSpec.o ../types/Matrix.o -o ../../bin/arimaforecast -lamath $(LIBS_LAPACK)

exe-cuda:
	$(CC) $(INCLUDES) $(LIBDIRS) $(LIBDIRS_CUDA) ./arimafit.o ./ARIMAModel.o ./ModelSpec.o ../types/Matrix.o -o ../../bin/arimafit -lamath $(LIBS_LAPACK) -lcublas
	$(CC) $(INCLUDES) $(LIBDIRS)  $(LIBDIRS_CUDA) ./arimaforecast.o ./ARIMAModel.o ./ModelSpec.o ../types/Matrix.o -o ../../bin/arimaforecast -lamath $(LIBS_LAPACK) -lcublas

clean:
	rm -f *.o;
