/********************************************************************
charstack.c
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include "types/charstack.h"

void init_cs(struct charstack* stack){
	stack->pHead = NULL;
	stack->length = 0;
}

void push_cs(struct charstack* stack, char byte){
	struct charstack_node* pNew;

	pNew = (struct charstack_node*)malloc(sizeof(struct charstack_node));
	pNew->data = byte;
	pNew->pNext = stack->pHead;

	stack->pHead = pNew;

	(stack->length)++;
}

char pop_cs(struct charstack* stack){
	struct charstack_node* pNode;
	char byte;
	
	pNode = stack->pHead;
	stack->pHead = pNode->pNext;
	byte = pNode->data;
	free(pNode);

	(stack->length)--;

	return byte;
}

char* fill_str(struct charstack* stack){
	char* str;

	size_t len = stack->length;
	str = (char*)malloc((len+1)*sizeof(char));
	str[len] = '\0';

	unsigned int i = 0;
	while(stack->length > 0){
		str[len-i-1]  = pop_cs(stack);
		i++;
	}
	return str;
}
