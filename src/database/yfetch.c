/********************************************************************
yfetch.c
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include <unistd.h>
#include "database/fetchdata.h"

void print_usage(char* exe);

unsigned int isdate(char* string);

int main(int argc, char* argv[]){

	int c, index;
	char* str;
	char sym[8];
	char fn_out[128];
	FILE* output;

	char start_date[20], end_date[20];
	end_date[0] = '\0';

	unsigned short fetch_options = 0;

	while ((c = getopt (argc, argv, "qh:dwmo:")) != -1){
		switch(c){
			case 'q':
				if(fetch_options & FETCH_HISTORIC){
					print_usage(argv[0]);
					return 1;
				}else{
					fetch_options |= FETCH_QUOTE;
				}
				break;
			case 'h':
				if(fetch_options & FETCH_QUOTE){
					print_usage(argv[0]);
					return 1;
				}else{
					index = optind-1;
					str = argv[index];
					if(isdate(&(str[0])) && index < argc){
						str = argv[index];
						strcpy(start_date,str);
						index++;
                	}else{
						print_usage(argv[0]);
						return 1;
					}
					str = argv[index];
					if(isdate(&(str[0])) && index < argc){
						str = argv[index];
						strcpy(end_date,str);
						index++;
					}
					optind = index;
					fetch_options |= FETCH_HISTORIC;
				}
				break;
			case 'd':
				if(fetch_options & FETCH_WEEKLY || fetch_options & FETCH_MONTHLY){
					print_usage(argv[0]);
					return 1;
				}else{
					fetch_options |= FETCH_DAILY;
				}
				break;
			case 'w':
				if(fetch_options & FETCH_DAILY || fetch_options & FETCH_MONTHLY){
					print_usage(argv[0]);
					return 1;
				}else{
					fetch_options |= FETCH_WEEKLY;
				}
				break;
			case 'm':
				if(fetch_options & FETCH_DAILY || fetch_options & FETCH_WEEKLY){
					print_usage(argv[0]);
					return 1;
				}else{
					fetch_options |= FETCH_MONTHLY;
				}
				break;
			case 'o':
				index = optind-1;
				str = argv[index];
				index++;
				if(str[0] != '-'){
                    strcpy(fn_out,str);
					fetch_options |= FETCH_OUTPUTFILE;
                }else{
					print_usage(argv[0]);
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
        strcpy(sym,str);
	}else{
		print_usage(argv[0]);
		return 1;
	}

	if(fetch_options & FETCH_OUTPUTFILE) output = fopen(fn_out,"w");

	if(end_date[0] != '\0') fetch_data(sym,start_date,end_date,output,fetch_options);
	else                    fetch_data(sym,start_date,NULL,output,fetch_options);

	if(fetch_options & FETCH_OUTPUTFILE) fclose(output);	

	return 0;
}

void print_usage(char* exe){

	fprintf(stderr,"usage: %s [-q | -h start_date [end_date] ] [-d | -w | -y] [-o output file] sym\n",exe);

}

unsigned int isdate(char* str){

	char b[2];
	unsigned int m, d, y;

	int count = sscanf(str,"%u%c%u%c%u %*s",&y,&(b[0]),&m,&(b[1]),&d);

	return (count == 5);

}
