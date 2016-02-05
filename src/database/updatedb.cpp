/********************************************************************
updatedb.cpp
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>
#include "database/StockDBConnection.hpp"

#include <unistd.h>
#include <mysql/mysql.h>
#include "database/fetchdata.h"

void printUsage(char* exe);

int main(int argc, char* argv[]){

	int c, index;
	char* str;
	unsigned short opt = 0;
	StockDBConnection db;
	std::string tickerfile, host, user, password, database, referencedate;
	std::string sym;
	std::stringstream input;
	std::ifstream ifs;

	while ((c = getopt (argc, argv, "pt:")) != -1){
		switch(c){
			case 'p':
				if(opt & 2){
					printUsage(argv[0]);
					return 1;
				}else{
					opt |= 1;
				}
				break;
			case 't':
				index = optind-1;
				str = argv[index];
				index++;
				if(str[0] != '-' && !(opt & 1)){
                    			tickerfile = std::string(str);
					opt |= 2;
                		}else{
					printUsage(argv[0]);
					return 1;
				}
				optind = index;
				break;
		}
	}
	index = optind;
	if(index < argc){
        	str = argv[index];
        	index++;
        	host = std::string(str);
        }else{
		printUsage(argv[0]);
		return 1;
	}
	if(index < argc){
        	str = argv[index];
        	index++;
        	user = std::string(str);
        }else{
		printUsage(argv[0]);
		return 1;
	}
	if(index < argc){
        	str = argv[index];
        	index++;
        	password = std::string(str);
        }else{
		printUsage(argv[0]);
		return 1;
	}
	if(index < argc){
        	str = argv[index];
        	index++;
        	database = std::string(str);
        }else{
		printUsage(argv[0]);
		return 1;
	}
	if(index < argc){
        	str = argv[index];
        	index++;
        	referencedate = std::string(str);
        }else{
		printUsage(argv[0]);
		return 1;
	}

	if(opt & 1){
		char byte;
		while(std::cin.get(byte)){
			input << byte;
		}
		
	}else if(opt & 2){
		ifs.open(tickerfile.c_str(),std::ios::in);
		char byte;
		while(ifs.get(byte)){
			input << byte;
		}
		ifs.close();
	}

	db.connect(host.c_str(),user.c_str(),password.c_str(),database.c_str(),DB_OPT_VERBOSE);

	while(getline(input,sym,'\n')){

		std::cout << sym << "\n";

		struct StockData* data;
		size_t length;
		size_t limit = 1;
		char refdate[11];
		std::string startdate;
		unsigned int m, d, y;

		getDate(referencedate.c_str(),&m,&d,&y);

		sprintf(refdate,"%4d-%02u-%02u",y,m,d);
		db.getStockData(sym.c_str(),refdate,NULL,&data,&length,&limit);

		sscanf(data[0].date,"%4u-%2u-%2u",&y,&m,&d);
		delete[] data;

		sprintf(refdate,"%02u/%02u/%4u",m,d,y);
		startdate = std::string(refdate);

		std::cout << "Updating From: " << startdate << "\n";

		std::string filename;
		filename = "./Data/" + sym;
		FILE* output = fopen(filename.c_str(),"w+");

		fetch_data(sym.c_str(),startdate.c_str(),NULL,output,17);

		fclose(output);

		db.setStockData(sym.c_str(),filename.c_str());

		remove(filename.c_str());
		usleep(500);
	}

	db.disconnect();

	return 0;
}

void printUsage(char* exe){

	std::cerr << "Usage: " << exe;
	std::cerr << " [ -p | -t tickerfile ] sqlhost sqluser sqlpass sqldb begindate\n";

}
