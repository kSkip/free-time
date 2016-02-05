#include <iostream>
#include "database/StockDBConnection.hpp"

int main(int argc, char* argv[]){

	StockDBConnection db;
	struct StockData* data;
	size_t size;
	unsigned int i;

	db.connect("Uranium.local","kane","VV@lfenste1n","HistoricStockData",DB_OPT_VERBOSE);

	db.getStockData("AAPL","2015-04-05",NULL,&data,&size,NULL);

	for(i=0;i<size;i++){
		std::cout << data[i].date << " " << data[i].open << " " << data[i].high << " " << data[i].low << " " << data[i].close << " " << data[i].volume << " " << data[i].adjclose << "\n";
	}

	db.addTicker("CSX");

	db.setStockData("CSX","./Data/CSX.dat");

	delete[] data;
	db.disconnect();

	return 0;
}
