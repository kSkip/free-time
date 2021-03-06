/********************************************************************
populatedb.cpp
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "database/StockDBConnection.hpp"
#include "database/dbinfo.hpp"

#include <unistd.h>

enum inputType {PIPE, INPUT_FILE};

void printUsage(char* exe);

int main(int argc, char* argv[]){

	int c, index;
	char* str;
	char byte;

	StockDBConnection db;
	ConnectionType db_type = MYSQL_DB;

	std::string datafile, db_file, type, host, user, password, database, table, tickertable;
	inputType input_type = PIPE;

	std::stringstream input;
	std::ifstream ifs;
	std::ofstream ofs;

	while ((c = getopt(argc, argv, "pf:ms")) != -1){
		switch(c){
			case 'p':
				input_type = PIPE;
				break;
			case 'f':
				index = optind-1;
				str = argv[index];
				index++;
				if(str[0] != '-'){
                    datafile = std::string(str);
					input_type = INPUT_FILE;
                }else{
					printUsage(argv[0]);
					return 1;
				}
				optind = index;
				break;
			case 'm':
				db_type = MYSQL_DB;
				break;
			case 's':
				std::cerr << "SQLite3 not yet supported\n";
				return 1;
				db_type = SQLITE3_DB;
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

	if(input_type == PIPE){

		while(std::cin.get(byte)) input << byte;

		datafile = std::string("./.populate");

		ofs.open(datafile.c_str(),std::ios::out);
		ofs << input.rdbuf();
		ofs.close();

	}

	getDatabaseInfo(db_file,type,host,user,password,database,table,tickertable);

	if(db.connect(host.c_str(),user.c_str(),password.c_str(),database.c_str(),table.c_str(),DB_OPT_VERBOSE) < 0){
	
		std::cerr << "Connection Failure\n";
		db.disconnect();
		return 1;

	}

	if(db.setStockData(datafile.c_str()) < 0){

		std::cerr << "Error inserting data\n";

	}

	if(input_type == PIPE) remove(datafile.c_str());

	db.disconnect();

	return 0;

}

void printUsage(char* exe){

	std::cerr << "Usage: " << exe;
	std::cerr << " [ -p | -f datafile ] [ -m | -s ] db-file\n";

}
