#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "amath/min_dist.h"
#include "types/charstack.h"

int main(int argc, char* agrv[]){

	double* data = NULL;
	double val;
	char c;
	unsigned int size, i, j;
	struct charstack cs;

	init_cs(&cs);

	i = 0;
	while(scanf("%c",&c) != EOF){
		push_cs(&cs,c);
	}
	char* input = fill_str(&cs);
	char* str;

	i = 0;
	str = strtok(input,"\n");
	while(str){
		i++;
		sscanf(str,"%lf",&val);
		data = (double*)realloc(data,i*sizeof(double));
		data[i-1] = val;
		str = strtok(NULL,"\n");
	}
	size = i;

	double e = min_dist(data,size,2);

	double* step_func = (double*)malloc(8*sizeof(double));
	double min, max, step;
	min = data[0];
	max = data[0];
	step = data[0];
	step_func[0] = 0;
	j = 0;
	for(i=1;i<size;i++){
		if(data[i] > max){
			if( (data[i] - min)/2.0 > e){
				step_func[2*j+1] = step;
				step_func[2*(j+1)] = i;
				step_func[2*(j+1)+1] = step;
				step_func[2*(j+2)] = i;
				j+=2;
				max = data[i];
				min = data[i];
				step = data[i];
			}else{
				max = data[i];
				step = (max + min)/2.0;
			}
		}else if(data[i] < min){
			if( (max - data[i])/2.0 > e){
				step_func[2*j+1] = step;
				step_func[2*(j+1)] = i;
				step_func[2*(j+1)+1] = step;
				step_func[2*(j+2)] = i;
				j+=2;
				max = data[i];
				min = data[i];
				step = data[i];
			}else{
				min = data[i];
				step = (max + min)/2.0;
			}
		}
	}
	step_func[2*j+1] = step;
	step_func[2*(j+1)] = i-1;
	step_func[2*(j+1)+1] = step;

	for(i=0;i<4;i++){
		printf("%f %f\n",step_func[2*i],step_func[2*i+1]);
	}

	return 0;
}
