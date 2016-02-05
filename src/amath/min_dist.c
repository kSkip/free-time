/********************************************************************
min_dist.c
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "amath/min_dist.h"

void sort_increasing(double* points, double* sorted, unsigned int size);
unsigned int power2(unsigned int size);
unsigned int decide(double* points, unsigned int size, unsigned int k, double e);
unsigned int next_e(unsigned int i, double* points, unsigned int size, double e);
double delta(unsigned int i, unsigned int j, double* points, unsigned int size);

double min_dist(double* points, unsigned int size, unsigned int k){
	unsigned int i, j, pad;
	double* psorted;
	double* M;

	psorted = (double*)malloc(size*sizeof(double));

	sort_increasing(points,psorted,size);

	i = size;
	pad = 0;
	while(!power2(i)){
		i++;
		pad++;
	}
	
	M = (double*)malloc(i*i*sizeof(double));
	for(i=0;i<(size+pad);i++){
		if(i > (size-1)){
			for(j=0;j<(size+pad);j++){
				M[(size+pad)*i+j] = M[(size+pad)*(size-1)+j];
			}
		}else{
			for(j=0;j<(size+pad);j++){
				if(j > (size-1)){
					M[(size+pad)*i+j] = M[(size+pad)*i+size-1];
				}else{
					M[(size+pad)*i+j] = (psorted[i]-psorted[size-1-j])/2.0;
				}
			}
		}
	}
	double min_e = 0;
	for(i=0;i<(size+pad);i++){
		for(j=0;j<(size+pad);j++){
			if(M[(size+pad)*i+j] > min_e) min_e = M[(size+pad)*i+j];
			/*printf("%4.2f ",M[(size+pad)*i+j]);*/
		}
		/*printf("\n");*/
	}
	
	for(i=0;i<(size+pad);i++){
		for(j=0;j<(size+pad);j++){
			unsigned int step = decide(points,size,k,M[(size+pad)*i+j]);
			if(step && (M[(size+pad)*i+j] < min_e)){
				min_e = M[(size+pad)*i+j];
			}
			/*printf("%d ",step);*/
		}
		/*printf("\n");*/
	}

	free(psorted);
	free(M);

	return min_e;
}

void sort_increasing(double* points, double* sorted, unsigned int size){
	unsigned int i, j, k;
	double min;
	memcpy(sorted,points,size*sizeof(double));

	for(i=0;i<size;i++){
		min = sorted[i];
		k = i;
		for(j=i;j<size;j++){
			if(sorted[j] < min){
				min = sorted[j];
				k = j;	
			}
		}
		sorted[k] = sorted[i];
		sorted[i] = min;
	}

}

unsigned int power2(unsigned int size){
	return (size && !(size & (size-1)));
}

unsigned int decide(double* points, unsigned int size, unsigned int k, double e){
	unsigned int i = 0, j;

	for(j=0;j<k;j++){
		i = next_e(i,points,size,e);
		if(i == size) return 1;
	}

	return 0;

}

unsigned int next_e(unsigned int i, double* points, unsigned int size, double e){
	unsigned int k;

	if(delta(i,size-1,points,size) <= e){
		return size;
	}

	for(k=i+1;k<size;k++){
		if(delta(i,k,points,size) > e){
			return k;
		}
	}

	return 0;

}

double delta(unsigned int i, unsigned int j, double* points, unsigned int size){
	double max, min;
	unsigned int k;

	for(k=i;k<=j;k++){
		if(k==i){
			max = points[k];
			min = points[k];
		}else{
			if(points[k] > max){
				max = points[k];
			}
			if(points[k] < min){
				min = points[k];
			}
		}
	}

	return (max-min)/2.0;

}


