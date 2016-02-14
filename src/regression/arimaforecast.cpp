/********************************************************************
arimaforecast.cpp
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
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>
#include "regression/ARIMAModel.hpp"

#include <unistd.h>

#define OPT_PIPE 1
#define OPT_FILEIN 2
#define OPT_FILEOUT 4

void printUsage(char* exe);

int main(int argc, char* argv[]){

	int c, index;
	char* str;
	unsigned short opt = 0;
	std::string outfile, infile, modelfile, value;
	std::stringstream input;
	std::ifstream ifs;
	std::ofstream ofs;

	unsigned int projection, trials;
	int initial_time;

	while ((c = getopt (argc, argv, "pd:o:")) != -1){
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
		}
	}
	index = optind;
	if(index < argc){
        	str = argv[index];
        	index++;
        	modelfile = std::string(str);
        }else{
		printUsage(argv[0]);
		return 1;
	}
	if(index < argc){
        	str = argv[index];
        	index++;
        	projection = atoi(str);
        }else{
		printUsage(argv[0]);
		return 1;
	}
	if(index < argc){
        	str = argv[index];
        	index++;
        	trials = atoi(str);
        }else{
		printUsage(argv[0]);
		return 1;
	}
	if(index < argc){
        	str = argv[index];
        	index++;
        	initial_time = atoi(str);
        }else{
		printUsage(argv[0]);
		return 1;
	}

	ARIMAModel model;

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

	ModelSpec s;
	s.load(modelfile.c_str());
	model.setModelSpec(&s);

	std::vector<double> data;
	std::vector<double> innovations;
	bool needRes = bool(s.q > 0);

	while(getline(input,value,'\n')){

		if(needRes){

			std::string::size_type pos = value.find(' ');
			if(value.npos != pos){

				data.push_back(atof((value.substr(0,pos)).c_str()));
				innovations.push_back(atof((value.substr(pos+1)).c_str()));

			}

		}else{
			data.push_back(atof(value.c_str()));
		}

	}

	struct ForecastParameters params;
	params.projection = projection;
	params.trials = trials;
	model.setForecastParams(&params);

	if(innovations.size() > 0) model.forecast(&(data[0]),data.size(),&(innovations[0]),initial_time);
	else                       model.forecast(&(data[0]),data.size(),NULL,initial_time);

	double* expectation = new double[projection];
	double* upper       = new double[projection];
	double* lower       = new double[projection];

	model.getForecast(expectation,upper,lower);

	if(opt & OPT_FILEOUT) ofs.open(outfile.c_str(),std::ios::out);

	unsigned int i;

	for(i=0;i<projection;i++){

		std::stringstream line;
		line << lower[i] << " " << expectation[i] << " " << upper[i] << "\n";

		if(opt & OPT_FILEOUT) ofs << line.rdbuf();
		else                  std::cout << line.rdbuf();

	}

	if(opt & OPT_FILEOUT) ofs.close();

	return 0;
}

void printUsage(char* exe){

	std::cerr << "Usage: " << exe;
	std::cerr << " [ -p | -d data_file ] [ -o output_file ] model_file projection trials initial_time\n";

}
