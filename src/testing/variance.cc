#include <iostream>
#include <fstream>
#include <vector>
#include "statistics/statistics.h"

int main(int argc, char* argv[]){

	if(argc < 2){
		std::cerr << "usage: " << argv[0] << " filename\n";
		return 1;
	}

	std::ifstream ifs;
	std::vector<double> data;

	ifs.open(argv[1],std::ios::in);

	while(ifs.good()){
		double value;
		ifs >> value;
		data.push_back(value);
	}
	ifs.close();

	double m = mean(&(data[0]),data.size());

	std::cout << variance(&(data[0]),data.size(),m) << "\n";

	return 0;
}
