/********************************************************************
arimafit.cpp
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
#include <cstring>
#include <sstream>
#include <fstream>
#include <vector>
#include "regression/ARIMAModel.hpp"

#include <unistd.h>

#define FIT_OPT_PIPE 1
#define FIT_OPT_FILEIN 2
#define FIT_OPT_FILEOUT 4
#define FIT_OPT_OPTIMIZE 8
#define FIT_OPT_SUPPRESS 16

void printUsage(char* exe);

int main(int argc, char* argv[]){

	int c, index;
	char* str;
	unsigned short opt = 0;
	std::string outfile, infile, value;
	std::stringstream input;
	std::ifstream ifs;
	std::ofstream ofs;

	unsigned int arLags, maLags, differencing;
	unsigned int p, q, i;
	unsigned short fit_opt = 0;

	while ((c = getopt (argc, argv, "xsctpd:o:")) != -1){
		switch(c){
			case 'x':
				opt |= FIT_OPT_OPTIMIZE;
				break;
			case 's':
				opt |= FIT_OPT_SUPPRESS;
				break;
			case 'c':
				fit_opt |= ARIMAMODEL_FIT_CONST;
				break;
			case 't':
				fit_opt |= ARIMAMODEL_FIT_TREND;
				break;
			case 'p':
				if(!(opt & FIT_OPT_FILEIN)){ 
					opt |= FIT_OPT_PIPE;
				}else{
					printUsage(argv[0]);
					return 1;
				}
				break;
			case 'd':
				index = optind-1;
				str = argv[index];
				index++;
				if(str[0] != '-' && !(opt & FIT_OPT_PIPE)){
                    			infile = std::string(str);
					opt |= FIT_OPT_FILEIN;
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
					opt |= FIT_OPT_FILEOUT;
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
        	arLags = atoi(str);
        }else{
		printUsage(argv[0]);
		return 1;
	}
	if(index < argc){
        	str = argv[index];
        	index++;
        	maLags = atoi(str);
        }else{
		printUsage(argv[0]);
		return 1;
	}
	if(index < argc){
        	str = argv[index];
        	index++;
        	differencing = atoi(str);
        }else{
		printUsage(argv[0]);
		return 1;
	}

	if(opt & FIT_OPT_PIPE){
		char byte;
		while(std::cin.get(byte)){
			input << byte;
		}
	}else if(opt & FIT_OPT_FILEIN){
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

	std::vector<ARIMAModel> models;
	if(opt & FIT_OPT_OPTIMIZE){
		for(p=0;p<=arLags;p++){
			for(q=0;q<=maLags;q++){
 				models.push_back(ARIMAModel(p,differencing,q));
			}
		}
	}else{
		models.push_back(ARIMAModel(arLags,differencing,maLags));
	}

	//If option -x is specified, we go through all the range of parameters to see which is best
	double* res = new double[data.size()];
	double* current_res = new double[data.size()];
	double min_aic;
	unsigned int bestModel;

	for(i=0;i<models.size();i++){
		
		models[i].fit(&(data[0]),data.size(),current_res,fit_opt);
		if(i == 0){
			min_aic = models[i].getAIC();
			memcpy(res,current_res,data.size()*sizeof(double));
			bestModel = 0;
		}else if(models[i].getAIC() < min_aic){
			min_aic = models[i].getAIC();
			memcpy(res,current_res,data.size()*sizeof(double));
			bestModel = i;
		}
	}

	ModelSpec s;
	models[bestModel].getModelSpec(&s);


	if(opt & FIT_OPT_FILEOUT){
		s.save(outfile.c_str());
	}else{
		s.print();
	}

	if(!(opt & FIT_OPT_SUPPRESS)){
	for(i=0;i<data.size();i++){

		std::cout << data[i] << " " << res[i] << "\n";

	}
	}

	delete[] res;
	delete[] current_res;

	return 0;
}

void printUsage(char* exe){

	std::cerr << "Usage: " << exe;
	std::cerr << " [ -xsct ] [ -p | -d data_file ] [ -o output_file ] num_ar_lags num_ma_lags num_differencing\n";

}
