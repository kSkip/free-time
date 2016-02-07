#free-time

free-time is a suite of time series analysis software provided as a collection of command line executables. Thus, they are easily used with a unix-like shell environment. The scripts provided for various calculations are written as Korn shell scripts. The Korn shell supports floating point arithmatic, which is why I chose it. However, any shell or interpreter is suitable (e.g. Bash, Python, etc.). Furthermore, executables for querying and storing data obtained from Yahoo! Finance are available. The schemas are fixed and the classes that interface with the database support only MySQL for now. Database and configuration scripts are written as Bourne shell scripts for compatiblity.

##Building

####Prerequisites
A C and a C++ compiler are required. The default are the gnu compilers, gcc and g++.
The following libraries need to be installed on the system:
  * LAPACK
  * LAPACKE (for the C interface)
  * cURL-dev
  * mysqlclient-dev
 
The following programs need to be installed for the provided scripts:
  * gnuplot
  * mysql (client)

Open the make.inc file provided with a text editor to indicate the locations of these libraries and the header files.

####Compiling
cd into the root directory of free-time and use the make target *all*.
```bash
cd path/to/free-time
make all
```
If compilation and linking have finished without any errors, you should find all the binaries for free-time in a folder named *bin*.

##Setup a Database
Skip this step and jump to the glossary if you are using a previous configured database, or you have access to your own source of data. As of now, only MySQL databases are supported for all the database management programs. If you have a running SQL server, simply run the script *configure.sh* in the root directory. The script is interactive and will setup a file named .dbconf which contains the credentials and database name that you will used for programs such as *pull*.  

Now, it's time to obtain the historical stock market data from Yahoo! Finance. Open the script BuildDatabase.sh and set the variable BEGINDATE to the earliest date you wish to retrieve data. The default is Jan. 1st 1970. Then run BuildDatabase.sh. This can take hours depending on how far back you set BEGINDATE.  

Once this is finished you will have a database with a table populated will the date, ticker, exchange, open, high, low, close, volume, and adjusted close price of every stock in the NYSE and the NASDAQ.
