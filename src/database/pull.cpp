/********************************************************************
pull.cpp
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <unistd.h>
#include "database/StockDBConnection.hpp"
#include "database/dbinfo.hpp"

#define OPT_DATE 1
#define OPT_OPEN 2
#define OPT_HIGH 4
#define OPT_LOW 8
#define OPT_CLOSE 16
#define OPT_VOLUME 32
#define OPT_ADJCLOSE 64
#define OPT_OUTFILE 128
#define OPT_ENDDATE 256

void printUsage(char* exe);

int main(int argc, char* argv[]){

	int c, index;
	char* str;
	unsigned short opt = 0;
	StockDBConnection db;
	std::string outfile, db_file, type, host, user, password, database, table, tickertable;
    std::string sym, startdate, enddate;
	std::ofstream ofs;

	while ((c = getopt (argc, argv, "dohlcvaf:ms")) != -1){
		switch(c){
			case 'd':
				opt |= OPT_DATE;
				break;
			case 'o':
				opt |= OPT_OPEN;
				break;
			case 'h':
				opt |= OPT_HIGH;
				break;
			case 'l':
				opt |= OPT_LOW;
				break;
			case 'c':
				opt |= OPT_CLOSE;
				break;
			case 'v':
				opt |= OPT_VOLUME;
				break;
			case 'a':
				opt |= OPT_ADJCLOSE;
				break;
			case 'f':
				index = optind-1;
				str = argv[index];
				index++;
				if(str[0] != '-'){
                    outfile = std::string(str);
					opt |= OPT_OUTFILE;
                }else{
					printUsage(argv[0]);
					return 1;
				}
				optind = index;
				break;
			case 'm':
				break;
			case 's':
				break;
		}
	}
	index = optind;
	if(index < argc){
        str = argv[index];
        index++;
        db_file = std::string(str);
	}else{
		printUsage(argv[0]);
		return 1;
	}
	if(index < argc){
        str = argv[index];
        index++;
        sym = std::string(str);
	}else{
		printUsage(argv[0]);
		return 1;
	}
	if(index < argc){
        str = argv[index];
        index++;
        startdate = std::string(str);
	}else{
		printUsage(argv[0]);
		return 1;
	}
	if(index < argc){
        str = argv[index];
        index++;
        enddate = std::string(str);
		opt |= OPT_ENDDATE;
	}

	getDatabaseInfo(db_file,type,host,user,password,database,table,tickertable);

	while(db.connect(host.c_str(),user.c_str(),password.c_str(),database.c_str(),table.c_str(),0) < 0){
		std::cerr << "Connection failure\n";	
	}

	struct StockData* data;
	size_t length;

	if(opt & OPT_ENDDATE) db.getStockData(sym.c_str(),startdate.c_str(),enddate.c_str(),&data,&length,NULL);
	else                  db.getStockData(sym.c_str(),startdate.c_str(),NULL,&data,&length,NULL);

	if(opt & OPT_OUTFILE) ofs.open(outfile.c_str(),std::ios::out);

	unsigned int i;
	for(i=0;i<length;i++){

		std::stringstream line;

		if(opt & OPT_DATE)     line << data[i].date   << " ";

		if(opt & OPT_OPEN)     line << data[i].open   << " ";

		if(opt & OPT_HIGH)     line << data[i].high   << " ";

		if(opt & OPT_LOW)      line << data[i].low    << " ";

		if(opt & OPT_CLOSE)    line << data[i].close  << " ";

		if(opt & OPT_VOLUME)   line << data[i].volume << " ";

		if(opt & OPT_ADJCLOSE) line << data[i].adjclose;

		line << "\n";

		if(opt & OPT_OUTFILE)  ofs << line.rdbuf();
		else                   std::cout << line.rdbuf();

	}

	if(opt & OPT_OUTFILE) ofs.close();
	delete[] data;

	db.disconnect();

	return 0;

}

void printUsage(char* exe){

	std::cerr << "Usage: " << exe;
	std::cerr << " [ -dohlcva ] [ -f output_file ]  [ -m | -s ] db-file ticker startdate [enddate]\n";

}
