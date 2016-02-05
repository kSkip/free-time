/********************************************************************
ModelSpec.cpp
Copyright (c) 2015, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include "regression/ARIMAModel.hpp"
#include <iostream>
#include <fstream>

ModelSpec::ModelSpec(){
	p = 0;
	q = 0;
	d = 0;
	c = 0;
	var = 0;
}

void ModelSpec::print(){

	std::cout << "p: " << p << "\n";
	std::cout << "q: " << q << "\n";
	std::cout << "d: " << d << "\n";
	unsigned int i;
	std::cout << "psi: ";
	for(i=0;i<p;i++){
		std::cout << psi[i] << " ";
	}
	std::cout << "\ntheta: ";
	for(i=0;i<q;i++){
		std::cout << theta[i] << " ";
	}
	std::cout << "\n";
	std::cout << "c: " << c << "\n";
	std::cout << "beta: " << beta << "\n";
	std::cout << "var: " << var << "\n";

}

int ModelSpec::save(const char* filename){
	unsigned int i;
	std::fstream ofs;

	ofs.open(filename,std::fstream::out);

	ofs << p << " " << d << " " << q << "\n";
	for(i=0;i<p;i++){
		ofs << psi[i] << " ";
	}
	ofs << "\n";
	for(i=0;i<q;i++){
		ofs << theta[i] << " ";
	}
	ofs << "\n";

	ofs << c << "\n";

	ofs << beta << "\n";
	
	ofs << var << "\n";

	ofs.close();

	return 0;

}

int ModelSpec::load(const char* filename){
	unsigned int i;
	std::fstream ifs;

	ifs.open(filename,std::fstream::in);

	ifs >> p >> d >> q;
	psi.resize(p);
	for(i=0;i<p;i++){
		ifs >> psi[i];
	}
	theta.resize(q);
	for(i=0;i<q;i++){
		ifs >> theta[i];
	}

	ifs >> c;

	ifs >> beta;

	ifs >> var;

	ifs.close();

	return 0;
}
