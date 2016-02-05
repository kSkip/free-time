/********************************************************************
StockDBConnection.cpp
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include "database/StockDBConnection.hpp"

//format strings used for the SQL statements
static char create_table[] = "CREATE TABLE `%s` (`sym` VARCHAR(8), `date` DATE, `open` FLOAT UNSIGNED, `high` FLOAT UNSIGNED, `low` FLOAT UNSIGNED, `close` FLOAT UNSIGNED, `volume` INT UNSIGNED, `adjclose` FLOAT UNSIGNED)";

static char insert_infile[] = "LOAD DATA LOCAL INFILE '%s' INTO TABLE `%s` COLUMNS TERMINATED BY ',' LINES TERMINATED BY '\n'";

static char insert_record[] = "INSERT INTO `%s` (`sym`,`date`,`open`,`high`,`low`,`close`,`volume`,`adjclose`) VALUES('%s','%s','%f','%f','%f','%f','%d','%f')";

//constructor
StockDBConnection::StockDBConnection(){
	mysql_init(&connection);
	connected = false;
}

//destructor
StockDBConnection::~StockDBConnection(){
	if(connected) disconnect();
}

//connect to MySQL database
int StockDBConnection::connect(const char* host, const char* user, const char* password, const char* database, const char* table, unsigned short options){

	mysql_options(&connection,MYSQL_OPT_LOCAL_INFILE,0);

	if(options & DB_OPT_INITIAL){

		if(!mysql_real_connect(&connection,host,user,password,NULL,0,NULL,0)){

			std::cerr << "Error " << mysql_errno(&connection) << ": ";
			std::cerr << mysql_error(&connection) << "\n";

			return -1;

		}

		if(createDatabase(database) < 0) return -1;

		if(createRecordsTable(table) < 0) return -1;

	}else{

		if(!mysql_real_connect(&connection,host,user,password,database,0,NULL,0)){

			std::cerr << "Error " << mysql_errno(&connection) << ": ";
			std::cerr << mysql_error(&connection) << "\n";

			return -1;

		}

	}

	connected = true;
	type = MYSQL_DB;
	tableName = std::string(table);

	if(options & DB_OPT_VERBOSE){
		std::cout << "MySql Version: " << mysql_get_server_info(&connection) << "\n";
		std::cout << "MySQL Connection Info: " << mysql_get_host_info(&connection) << "\n";
	}

	return 0;

}

//connect to SQLite3 database
int StockDBConnection::connect(const char* database, const char* table, unsigned short options){

	std::cerr << "SQLite3 not yet supported\n";
	return -1;

}

//disconnect current database
void StockDBConnection::disconnect(){

	mysql_close(&connection);
	connected = false;

}

int StockDBConnection::createDatabase(const char* db_name){

	char query[256];

	sprintf(query,"CREATE DATABASE %s",db_name);

	if(mysql_query(&connection,query)){

		std::cerr << "Error " << mysql_errno(&connection) << ": ";
		std::cerr << mysql_error(&connection) << "\n";

		return -1;

	}

	sprintf(query,"USE %s",db_name);

	if(mysql_query(&connection,query)){

		std::cerr << "Error " << mysql_errno(&connection) << ": ";
		std::cerr << mysql_error(&connection) << "\n";

		return -1;

	}


	return 0;

}

/*create the records table with the format of yahoo finace
plus two columns for the ticker and exchange*/
int StockDBConnection::createRecordsTable(const char* table_name){

	char query[256];

	sprintf(query,create_table,table_name);

	if(mysql_query(&connection,query)){

		std::cerr << "Error " << mysql_errno(&connection) << ": ";
		std::cerr << mysql_error(&connection) << "\n";

		return -1;

	}

	//we need an index. the combination of sym & date are unique together
	sprintf(query,"CREATE UNIQUE INDEX stock_day ON %s (`sym`,`date`)",table_name);

	if(mysql_query(&connection,query)){

		std::cerr << "Error " << mysql_errno(&connection) << ": ";
		std::cerr << mysql_error(&connection) << "\n";

		return -1;

	}

	return 0;

}

int StockDBConnection::getStockData(const char* ticker, const char* startdate, const char* enddate, struct StockData** data, size_t* length, size_t* limit){

	std::string query;
	unsigned int rows, cols, i;
	MYSQL_ROW row;

	*length = 0;

	query += "SELECT * FROM `";
	query += tableName;
	query += "` WHERE sym='";
	query += ticker;
	query += "' AND date>'";
	query += startdate;

	if(enddate){
		query += "' AND date<'";
		query += enddate;
	}

	query += "' ORDER BY date DESC";

	if(limit){
		char num[8];
		query += " LIMIT ";
		sprintf(num,"%zu",*limit);
		query += std::string(num);
	}

	if(mysql_query(&connection,query.c_str())){

		std::cerr << "Error " << mysql_errno(&connection) << ": ";
		std::cerr << mysql_error(&connection) << "\n";

		return -1;

	}

	MYSQL_RES* result = mysql_store_result(&connection);

	if(!result){
		std::cout << "Empty Set\n";
		return -1;
	}

	rows = mysql_num_rows(result);
	cols = mysql_num_fields(result);

	if(cols < 8){
		std::cerr << "Error Too Few Columns in Set\n";
		return -1;
	}


	*data = (struct StockData*)malloc(rows*sizeof(struct StockData));
	i=0;
	while((row = mysql_fetch_row(result))){

		memcpy((*data)[i].date,row[1],11*sizeof(char));
		(*data)[i].open     = atof(row[2]);
		(*data)[i].high     = atof(row[3]);
		(*data)[i].low      = atof(row[4]);
		(*data)[i].close    = atof(row[5]);
		(*data)[i].volume   = atoi(row[6]);	
		(*data)[i].adjclose = atof(row[7]);
		++i;
		if(i == rows) break;

	}
	*length = i;

	if(result){
		mysql_free_result(result);
		result = NULL;
	}

	return 0;

}

int StockDBConnection::setStockData(const char* ticker, struct StockData* data, size_t* length){
	
	char query[256];
	unsigned int i;

	for(i=0;i<*length;i++){

		sprintf(query,insert_record,tableName.c_str(),ticker,data[i].date,data[i].open,data[i].high,data[i].low,data[i].close,data[i].volume,data[i].adjclose);

		if(mysql_query(&connection,query)){

			std::cerr << "Error " << mysql_errno(&connection) << ": ";
			std::cerr << mysql_error(&connection) << "\n";

			return -1;

		}

		std::cout << "Affected Rows: " << mysql_affected_rows(&connection) << "\n";
	
	}

	return 0;

}

int StockDBConnection::setStockData(const char* filename){

	char query[256];

	sprintf(query,insert_infile,filename,tableName.c_str());

	if(mysql_query(&connection,query)){

		std::cerr << "Error " << mysql_errno(&connection) << ": ";
		std::cerr << mysql_error(&connection) << "\n";

		return -1;

	}

	std::cout << "Affected Rows: " << mysql_affected_rows(&connection) << "\n";

	return 0;

}
