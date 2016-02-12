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
  
##Commands
**yfetch:** Connect to Yahoo! Finance historic data website and pull either historic stock quotes or current quotes, then prints the data to stdout or a file.  

*Usage:* yfetch [-q | -h start_date [end_date] ] [-d | -w | -m] [-o output file] sym  

*Arguments:*  
sym: ticker symbol for desired stock

*Options:*  
-q: fetch current quote  
-h: fetch historic data starting from the date indicated by start_date an optionally end_date. If end_date is not specified then the current system date is used.  
-d: fetch daily historic data  
-w: fetch weekly historic data  
-m: fetch monthly historic data  

**pull:** Connect to a SQL database and obtain the historic stock data stored there. This program is specifically desinged to pull data from one type of table schema created and initialized by the scripts *configure.sh* and *BuildDatabase.sh*.  

*Usage:* pull [ -dohlcva ] [ -f output_file ]  [ -m | -s ] db-file ticker startdate [enddate]  

*Arguments:*  
db-file: database configuration file. If using *configure.sh* to setup database, this is *.dbconf*.  
ticker: ticker symbol for desired stock  
startdate: the starting date for the obtained data set  
enddate(optional): the ending date for the obtained data set. If none then the current system date is used.

*Options:*  
-d: pull date  
-o: pull open price  
-h: pull high price  
-l: pull low price  
-c: pull close price  
-v: pull volume  
-a: pull adjusted close price
-f: print to a file
-m: MySQL Database
-s: SQLite3 Database (Not Yet Supported)

**arimafit:**  Fit an autoregressive-integrated-moving-average (ARIMA) model to time series data provided by a file or stdin.  

*Usage:* arimafit [ -xsct ] [ -p | -d data_file ] [ -o output_file ] num_ar_lags num_ma_lags num_differencing  

*Arguments:*  
num_ar_lags: the number of autoregressive lags to include in the model  
num_ma_lags: the number of moving average lags to include in the model  
num_differencing: the number of times to difference the data before fitting (a.k.a the intgeration level)  

*Options:*  
-x: optimixe the model by determining the model parameters  
-s: suppress to output of the residuals  
-c: include a constant term in the model  
-t: include a deterministic trend term to the model  
-p: read data from pipe (i.e. stdin)  
-d: read data from file  
-o: write output model to file instead of stdout  

**arimaforecast:**  Forecast a time series using a specified ARIMA model using a Monte Carlo simulation.  

*Usage:* arimaforecast [ -p | -d data_file ] [ -o output_file ] model_file projection trials initial_time  

*Arguments:*  
model_file: the file containing information about the model to use. This file is usually produced by *arimafit*.  
projection: the number of steps in time to project  
trials: the number of simulations to preform  
initial_time: the time of the input time series to start sampling for forecasting  

*Options:*  
-p: read the sampling time series from pipe  
-d: read the sampling time series from file  
-o: output projections to file rather than stdout  
