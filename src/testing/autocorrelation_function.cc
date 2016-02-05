#include <iostream>
#include <fstream>
#include <vector>
#include "statistics/statistics.h"

int main(int argc, char* argv[]){

	if(argc < 3){
		std::cerr << "usage: " << argv[0] << " filename num_lags\n";
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
	unsigned int lag = atoi(argv[2]);

	double* acf = new double[lag];

	autocorrelation_function(&(data[0]),data.size(),lag,m,acf);

	unsigned int i;
	for(i=0;i<lag;i++){
		std::cout << i+1 << " " << acf[i] << "\n";
	}

	delete[] acf;

	return 0;
}
