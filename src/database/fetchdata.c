/********************************************************************
fetchdata.c
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include "database/fetchdata.h"

static char base_url_q[] = {"http://finance.yahoo.com/d/quotes.csv?s=%s&f=snpl1"};
static char base_url_h[] = {"http://ichart.finance.yahoo.com/table.csv?s=%s&a=%d&b=%d&c=%d&d=%d&e=%d&f=%d&g=%c"};

static char months[12][3] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

/*
 * Returns integer value associated with month abbreviation
 */
int month(char* month_string){

	size_t i;

	for(i=0;i<12;++i){

		if(!strcmp(month_string,months[i])) return i + 1;

	}

	return 0;
}

int getDate(const char* string, unsigned int* m_in, unsigned int* d_in, unsigned int* y_in){

	char b[2];
	char date_string[32], month_string[3];

	if(string){

		int count = sscanf(string,"%u%c%u%c%u",y_in,&(b[0]),m_in,&(b[1]),d_in);

		if(count == 5 && b[0] == '-' && b[1] == '-') return 1;
		else                                         return 0;

	}else{

		time_t now = time(NULL);
		ctime_r(&now,date_string);
		sscanf(date_string,"%*s %s %d %*s %d\n",month_string,d_in,y_in);
		*m_in = month(month_string);

		return 1;

	}

}

char fetchInterval(unsigned short options){

	if(options & FETCH_DAILY)   return 'd';

	if(options & FETCH_WEEKLY)  return 'w';

	if(options & FETCH_MONTHLY) return 'm';

	return 'd';

}

/*
 * copies the contents from the file obtain by the curl http request
 */
size_t write_data(FILE* in, FILE* out, unsigned short options){

	unsigned int i, pos;

	fseek(in,0L,SEEK_END);
	long int size = ftell(in);
	fseek(in,0L,SEEK_SET);

	char* ptr = (char*)malloc(size*sizeof(char));

	fread(ptr,sizeof(char),size,in);

	if(strstr(ptr,"404 Not Found")){
		return 0;
	}
	
	pos = 0;
	if(options & FETCH_HISTORIC){

		/*replace commas with spaces*/
		/*for(i=0;i<size;i++){
			if(ptr[i] == ','){
				ptr[i] = ' ';
			}
		}*/

		i = 0;
		while(ptr[i] != '\n') i++; /*skip header line*/
		pos = i+1;

	}

	size_t ret = fwrite(&(ptr[pos]),sizeof(char),size-pos,out);
	free(ptr);

	return ret;

}

/*
 * options are bitwise ORs of the commandline flags qhdwmo
 */
void fetch_data(const char* sym, const char* start_date, const char* end_date, FILE* output_fp, unsigned short options){

	char url[128];
	unsigned int s_m, s_d, s_y, e_m, e_d, e_y;

	getDate(NULL,&e_m,&e_d,&e_y);
	e_m--; /* yahoo finance uses zero based month numbers */

	CURL* curl;
	FILE* output;

	curl = curl_easy_init();
	if(curl){

		if(options & FETCH_QUOTE){

			sprintf(url,base_url_q,sym);
			
			curl_easy_setopt(curl,CURLOPT_URL,url);

		}

		if(options & FETCH_HISTORIC){

			getDate(start_date,&s_m,&s_d,&s_y);
			s_m--; /*yahoo finance uses zero based month numbers*/

			if(end_date){
				getDate(end_date,&e_m,&e_d,&e_y);
				e_m--; /*yahoo finance uses zero based month numbers*/
			}

			/*encode the options into the base url*/
			sprintf(url,base_url_h,sym,s_m,s_d,s_y,e_m,e_d,e_y,fetchInterval(options));

			curl_easy_setopt(curl,CURLOPT_URL,url);

		}

		if(options & FETCH_OUTPUTFILE) output = output_fp;
		else                           output = stdout;

		FILE* f = fopen("curlhtml","w+");

		curl_easy_setopt(curl,CURLOPT_FILE,f);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		curl_easy_perform(curl);

		write_data(f,output,options);

		fclose(f);
		remove("./curlhtml");

		curl_easy_cleanup(curl);
	}

}
