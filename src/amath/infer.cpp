/********************************************************************
infer.cpp
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
#include "amath/amath.h"

#define OPT_PIPE 1
#define OPT_FILEIN 2
#define OPT_FILEOUT 4
#define OPT_DIFFERENCE 8
#define OPT_DETREND 16

void printUsage(char* exe);

int main(int argc, char* argv[]){

	int c, index;
	char* str;
	unsigned short opt = 0;
	std::string outfile, infile, modelfile, value;
	std::stringstream input;
	std::ifstream ifs;
	std::ofstream ofs;

	unsigned int max_lags, d;
	double* differenced;
	double* detrended;
	double a, b;

	while ((c = getopt (argc, argv, "pd:o:x:t")) != -1){
		switch(c){
			case 'p':
				if(!(opt & OPT_FILEIN)){ 
					opt |= OPT_PIPE;
				}else{
					printUsage(argv[0]);
					return 1;
				}
				break;
			case 'd':
				index = optind-1;
				str = argv[index];
				index++;
				if(str[0] != '-' && !(opt & OPT_PIPE)){
                    			infile = std::string(str);
					opt |= OPT_FILEIN;
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
					opt |= OPT_FILEOUT;
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
					opt |= OPT_DIFFERENCE;
                		}else{
					printUsage(argv[0]);
					return 1;
				}
				optind = index;
				break;
			case 't':
				opt |= OPT_DETREND;
				break;
		}
	}
	index = optind;
	if(index < argc){
        	str = argv[index];
        	index++;
        	max_lags = atoi(str);
        }else{
		printUsage(argv[0]);
		return 1;
	}

	if(opt & OPT_PIPE){
		char byte;
		while(std::cin.get(byte)){
			input << byte;
		}
	}else if(opt & OPT_FILEIN){
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

	double p, q;

	if(opt & OPT_DIFFERENCE){

		differenced = new double[data.size()-d];

		if(opt & OPT_DETREND){

			detrended = new double[data.size()-d];

			detrend(differenced,data.size()-d,&a,&b,detrended);

			infer_lags(detrended,data.size()-d,max_lags,&p,&q);

			delete[] detrended;

		}else{

			infer_lags(differenced,data.size()-d,max_lags,&p,&q);

		}

		delete[] differenced;

	}else{

		if(opt & OPT_DETREND){

			detrended = new double[data.size()];

			detrend(&(data[0]),data.size(),&a,&b,detrended);

			infer_lags(detrended,data.size(),max_lags,&p,&q);

			delete[] detrended;

		}else{

			infer_lags(&(data[0]),data.size(),max_lags,&p,&q);

		}

	}

	if(opt & OPT_FILEOUT){
		ofs.open(outfile.c_str(),std::ios::out);

		ofs << p << " " << q << "\n";

		ofs.close();
	}else{

		std::cout << p << " " << q << "\n";

	}

	return 0;
}

void printUsage(char* exe){

	std::cerr << "Usage: " << exe;
	std::cerr << " [ -p | -d data_file ] [ -o output_file ] [ -x differencing ] [ -t ] max_lags\n";

}
