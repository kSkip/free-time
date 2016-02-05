/********************************************************************
StockDBConnection.hpp
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#ifndef STOCKDBCONNECTION_HPP
#define STOCKDBCONNECTION_HPP

#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <mysql/mysql.h>
//#include "sqlite3.h"

#define DB_OPT_VERBOSE 1
#define DB_OPT_INITIAL 2

enum ConnectionType {MYSQL_DB, SQLITE3_DB};

struct StockData{
	char date[11];
	float open;
	float close;
	float high;
	float low;
	unsigned int volume;
	float adjclose;
};

class StockDBConnection{
	public:
		StockDBConnection();
		~StockDBConnection();

		int connect(const char* host, const char* user, const char* password, const char* database, const char* table, unsigned short options);
		int connect(const char* database, const char* table, unsigned short options);
		void disconnect();

		int createRecordsTable(const char* table_name);
		int createDatabase(const char* db_name);

		int getStockData(const char* ticker, const char* startdate, const char* enddate, struct StockData** data, size_t* length, size_t* limit);
		int setStockData(const char* ticker, struct StockData* data, size_t* length);
		int setStockData(const char* filename);
	private:
		MYSQL connection;
		//sqlite3* db;
		ConnectionType type;
		bool connected;
		std::string tableName;
};

#endif
