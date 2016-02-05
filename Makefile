include make.inc

install:
	cp -r ./bin /usr/local

all: amath database test regression types
	if [ ! -d bin ]; then mkdir bin; fi
	if [ ! -d lib ]; then mkdir lib; fi
	if [ ! -d testing ]; then mkdir testing; fi
	cd src/amath; make exe;
	cd src/database; make exe;
	cd src/regression; make exe;
	cd src/testing; make exe;

amath:
	cd src/amath; make all;

database:
	cd src/database; make all;

test:
	cd src/testing; make all;

regression:
	cd src/regression; make all;

types:
	cd src/types; make all;



clean:
	cd bin; rm -f *;
	cd lib; rm -f *;
	cd src/regression; make clean;
	cd src/types; make clean;
	cd src/amath; make clean;
	cd src/database; make clean;
	cd src/testing; make clean;
