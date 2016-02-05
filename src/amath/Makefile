include ../../make.inc

all: libamath
	$(CC) $(CFLAGS) $(INCLUDES) acf.cpp
	$(CC) $(CFLAGS) $(INCLUDES) pacf.cpp
	$(CC) $(CFLAGS) $(INCLUDES) stepfit.cpp
	$(CC) $(CFLAGS) $(INCLUDES) infer.cpp
	$(CC) $(CFLAGS) $(INCLUDES) dftest.cpp
	$(CC) $(CFLAGS) $(INCLUDES) kpsstest.cpp
	$(CC) $(CFLAGS) $(INCLUDES) find_unit_root.cpp

exe:
	$(CC) $(INCLUDES) $(LIBDIRS) ./acf.o -o ../../bin/acf -lamath $(LIBS_LAPACK) 
	$(CC) $(INCLUDES) $(LIBDIRS) ./pacf.o -o ../../bin/pacf -lamath $(LIBS_LAPACK)
	$(CC) $(INCLUDES) $(LIBDIRS) ./stepfit.o -o ../../bin/stepfit -lamath $(LIBS_LAPACK)
	$(CC) $(INCLUDES) $(LIBDIRS) ./infer.o ../regression/ModelSpec.o -o ../../bin/infer -lamath $(LIBS_LAPACK)
	$(CC) $(INCLUDES) $(LIBDIRS) ./dftest.o -o ../../bin/dftest -lamath $(LIBS_LAPACK)
	$(CC) $(INCLUDES) $(LIBDIRS) ./kpsstest.o -o ../../bin/kpsstest -lamath $(LIBS_LAPACK)
	$(CC) $(INCLUDES) $(LIBDIRS) ./find_unit_root.o -o ../../bin/fur -lamath $(LIBS_LAPACK)

libamath:
	$(C) $(CFLAGS) $(INCLUDES) statistics.c min_dist.c estimation.c stationarity_tests.c
	$(AR) $(ARFLAGS) ../../lib/libamath.a statistics.o min_dist.o estimation.o stationarity_tests.o

clean:
	rm -f *.o;
