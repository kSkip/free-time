/********************************************************************
pacf.cpp
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
#include <vector>

#include <unistd.h>
#include "amath/statistics.h"

#define PACF_OPT_PIPE 1
#define PACF_OPT_FILEIN 2
#define PACF_OPT_FILEOUT 4
#define PACF_OPT_DIFFERENCE 8
#define PACF_OPT_DETREND 16

void printUsage(char* exe);

int main(int argc, char* argv[]){

	int c, index;
	char* str;
	unsigned short opt = 0;
	std::string outfile, infile, value;
	std::stringstream input;
	std::ifstream ifs;
	std::ofstream ofs;

	unsigned int numLags, d;
	double* differenced;
	double* detrended;
	double a, b;

	while ((c = getopt (argc, argv, "pd:o:x:t")) != -1){
		switch(c){
			case 'p':
				if(!(opt & PACF_OPT_FILEIN)){ 
					opt |= PACF_OPT_PIPE;
				}else{
					printUsage(argv[0]);
					return 1;
				}
				break;
			case 'd':
				index = optind-1;
				str = argv[index];
				index++;
				if(str[0] != '-' && !(opt & PACF_OPT_PIPE)){
                    			infile = std::string(str);
					opt |= PACF_OPT_FILEIN;
                		}else{
					printUsage(argv[0]);
					return 1;
				}
				optind = index;
				break;
			case 'o':
				index = optind-1;
				str = argv[index];
				index++;
				if(str[0] != '-'){
                    			outfile = std::string(str);
					opt |= PACF_OPT_FILEOUT;
                		}else{
					printUsage(argv[0]);
					return 1;
				}
				optind = index;
				break;
			case 'x':
				index = optind-1;
				str = argv[index];
				index++;
				if(str[0] != '-'){
                    			d = atoi(str);
					opt |= PACF_OPT_DIFFERENCE;
                		}else{
					printUsage(argv[0]);
					return 1;
				}
				optind = index;
				break;
			case 't':
				opt |= PACF_OPT_DETREND;
				break;
		}
	}
	index = optind;
	if(index < argc){
        	str = argv[index];
        	index++;
        	numLags = atoi(str);
        }else{
		printUsage(argv[0]);
		return 1;
	}

	if(opt & PACF_OPT_PIPE){
		char byte;
		while(std::cin.get(byte)){
			input << byte;
		}
	}else if(opt & PACF_OPT_FILEIN){
		ifs.open(infile.c_str(),std::ios::in);
		char byte;
		while(ifs.get(byte)){
			input << byte;
		}
		ifs.close();
	}else{
		printUsage(argv[0]);
		return 1;
	}

	std::vector<double> data;
	while(getline(input,value,'\n')){
		data.push_back(atof(value.c_str()));
	}

	double* pacf = new double[numLags];

	if(opt & PACF_OPT_DIFFERENCE){
		
		differenced = new double[data.size()-d];
		difference(&(data[0]),data.size(),d,differenced);
		
		double m = mean(differenced,data.size()-d);

		if(opt & PACF_OPT_DETREND){

			detrended = new double[data.size()-d];

			detrend(differenced,data.size()-d,&a,&b,detrended);

			partial_autocorrelation_function(detrended,data.size()-d,numLags,0,pacf);

			delete[] detrended;

		}else{

			partial_autocorrelation_function(differenced,data.size()-d,numLags,m,pacf);

		}

		delete[] differenced;

	}else{

		double m = mean(&(data[0]),data.size());

		if(opt & PACF_OPT_DETREND){

			detrended = new double[data.size()];

			detrend(&(data[0]),data.size(),&a,&b,detrended);

			partial_autocorrelation_function(detrended,data.size(),numLags,0,pacf);

			delete[] detrended;

		}else{

			partial_autocorrelation_function(&(data[0]),data.size(),numLags,m,pacf);

		}

	}

	if(opt & PACF_OPT_FILEOUT) ofs.open(outfile.c_str(),std::ios::out);
	unsigned int i;
	for(i=0;i<numLags;i++){
		std::stringstream line;
		line << pacf[i] << "\n";
		if(opt & PACF_OPT_FILEOUT){
			ofs << line.rdbuf();
		}else{
			std::cout << line.rdbuf();
		}
	}
	if(opt & PACF_OPT_FILEOUT) ofs.close();

	delete[] pacf;

	return 0;
}

void printUsage(char* exe){

	std::cerr << "Usage: " << exe;
	std::cerr << " [ -p | -d data_file ] [ -o output_file ] [ -x differencing ] [ -t ] num_lags\n";

}

