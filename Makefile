include make.inc

install:
	cp -r ./bin /usr/local

all: amath database regression types
	if [ ! -d bin ]; then mkdir bin; fi
	if [ ! -d lib ]; then mkdir lib; fi
	cd src/amath; make exe;
	cd src/database; make exe;
	cd src/regression; make exe;

all-cuda: amath database regression types-cuda cuda
	if [ ! -d bin ]; then mkdir bin; fi
	if [ ! -d lib ]; then mkdir lib; fi
	cd src/amath; make exe;
	cd src/database; make exe;
	cd src/regression; make exe-cuda;

amath:
	cd src/amath; make all;

database:
	cd src/database; make all;

regression:
	cd src/regression; make all;

types:
	cd src/types; make all;

types-cuda:
	cd src/types; make all-cuda;

cuda:
	cd src/cuda; make all;

clean:
	cd bin; rm -f *;
	cd lib; rm -f *;
	cd src/regression; make clean;
	cd src/types; make clean;
	cd src/amath; make clean;
	cd src/database; make clean;
