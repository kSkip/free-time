#include <iostream>
#include <fstream>
#include <vector>
#include "statistics/statistics.h"

int main(int argc, char* argv[]){

	if(argc < 4){
		std::cerr << "usage: " << argv[0] << " filename1 filename2 lag\n";
		return 1;
	}

	std::ifstream ifs;
	std::vector<double> data1, data2;

	ifs.open(argv[1],std::ios::in);
	while(ifs.good()){
		double value;
		ifs >> value;
		data1.push_back(value);
	}
	ifs.close();
	ifs.open(argv[2],std::ios::in);
	while(ifs.good()){
		double value;
		ifs >> value;
		data2.push_back(value);
	}
	ifs.close();

	double m1 = mean(&(data1[0]),data1.size());
	double m2 = mean(&(data2[0]),data2.size());
	unsigned int lag = atoi(argv[3]);

	std::cout << covariance(&(data1[0]),&(data2[0]),data1.size(),lag,m1,m2) << "\n";

	return 0;
}
