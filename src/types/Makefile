include ../../make.inc

all:
	$(CC) $(CFLAGS) $(INCLUDES) Matrix.cpp
	$(C) $(CFLAGS) $(INCLUDES) charstack.c

all-cuda:
	nvcc -c $(INCLUDES) -DCUDA Matrix.cpp
	$(C) $(CFLAGS) $(INCLUDES) charstack.c

clean:
	rm -f *.o
