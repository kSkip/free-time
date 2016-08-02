/********************************************************************
ANN.hpp
Copyright (c) 2016, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#ifndef ANN_HPP
#define ANN_HPP


#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>

#include <cstdlib>
#include <cmath>

#include "types/Matrix.hpp"

#ifndef TYPE_DVEC
#define TYPE_DVEC
typedef std::vector<double> dvec;
#endif

#ifndef TYPE_TABLE
#define TYPE_TABLE
typedef std::vector< std::vector<double> > Table;
#endif

/*
 * Base class for all neural network
 * implmentations in free-time applications
 */
class ANN {

    public:
        ANN();
        ~ANN();
        ANN(const ANN & rhs);

        virtual void define(const char* filename) =0;

        virtual void train(const Table& inputs, const Table& outputs) =0;

        virtual dvec predict(const dvec & inputs) =0;

    protected:

        static double sigmoid(double z);

        static double quad_dC_dA(double a, double out);

};

/*
 * Deep Artifical Neural Network:
 * All neurons feedforward output to
 * all other neurons in the next layer.
 * Multiple hidden layers can exist.
 */
class DNN : public ANN {

    public:

        DNN();
        DNN(const char* filename);
        ~DNN();
        DNN(const DNN & rhs);

        void define(const char* filename);

        void train(const Table& inputs, const Table& outputs);

        dvec predict(const dvec & inputs);

        std::string toString();

    private:

        /* This is the network */
        std::vector<Matrix> activations;
        std::vector<Matrix> weights;
        std::vector<Matrix> bias;

        std::vector<Matrix> wGradient;
        std::vector<Matrix> bGradient;

        double learningRate;

        /* Backpropagation functions */
        void backPropSet(const dvec& input, const dvec& output);

        void feedForward(const dvec & inputs);

};

#endif
