/********************************************************************
find_unit_root.cpp
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

void printUsage(char* exe);

int main(int argc, char* argv[]){

	int c, index;
	char* str;
	unsigned short opt = 0;
	std::string outfile, infile, value;
	std::stringstream input;
	std::ifstream ifs;
	std::ofstream ofs;

	unsigned int T;

	while ((c = getopt (argc, argv, "pd:v")) != -1){
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
		}
	}
	index = optind;
	if(index < argc){
        	str = argv[index];
        	index++;
        	T = atoi(str);
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

	unsigned int kpss_simple_res = kpss(&(data[0]),data.size(),T,KPSS_SIMPLE);

	unsigned int kpss_trend_res = kpss(&(data[0]),data.size(),T,KPSS_TREND);

	unsigned int df_simple_res = df(&(data[0]),data.size(),DF_SIMPLE);

	unsigned int df_const_res = df(&(data[0]),data.size(),DF_CONST);

	unsigned int df_const_trend_res = df(&(data[0]),data.size(),DF_CONST | DF_TREND);

	if( df_simple_res && !kpss_simple_res && !kpss_trend_res ){
		std::cout << "I(0),no const,no trend\n";
	}else if( df_const_res && !kpss_simple_res && !kpss_trend_res ){
		std::cout << "I(0),const,no trend\n";
	}else if( df_const_trend_res && !kpss_simple_res && !kpss_trend_res ){
		std::cout << "I(0),const,trend\n";
	}else if( kpss_simple_res && !df_simple_res && !df_const_res && !df_const_trend_res ){
		std::cout << "I(1),const,no trend\n";
	}else if( kpss_trend_res && !df_simple_res && !df_const_res && !df_const_trend_res ){
		std::cout << "I(1),const,trend\n";
	}else{
		std::cout << "I(n/a),n/a,n/a\n";
	}

	return 0;
}

void printUsage(char* exe){

	std::cerr << "Usage: " << exe;
	std::cerr << " [ -p | -d data_file ] kpss_reference\n";

}
