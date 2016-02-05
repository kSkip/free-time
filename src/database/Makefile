include ../../make.inc

all:
	$(C) $(CFLAGS) $(INCLUDES) fetchdata.c
	$(C) $(CFLAGS) $(INCLUDES) yfetch.c
	$(CC) $(CFLAGS) $(INCLUDES) StockDBConnection.cpp
	$(CC) $(CFLAGS) $(INCLUDES) dbinfo.cpp
	$(CC) $(CFLAGS) $(INCLUDES) populatedb.cpp
	$(CC) $(CFLAGS) $(INCLUDES) initializedb.cpp
	$(CC) $(CFLAGS) $(INCLUDES) pull.cpp

exe:
	$(C) $(INCLUDES) ./fetchdata.o ./yfetch.o -o ../../bin/yfetch -lcurl
	$(CC) $(INCLUDES) ./populatedb.o ./StockDBConnection.o ./dbinfo.o -o ../../bin/populatedb -lmysqlclient
	$(CC) $(INCLUDES) ./initializedb.o ./StockDBConnection.o ./dbinfo.o -o ../../bin/initializedb -lmysqlclient
	$(CC) $(INCLUDES) ./pull.o ./StockDBConnection.o ./dbinfo.o -o ../../bin/pull -lmysqlclient

clean:
	rm -f *.o;
