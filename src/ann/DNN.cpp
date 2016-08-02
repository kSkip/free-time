/********************************************************************
DNN.cpp
Copyright (c) 2016, Kane Scipioni
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

#include "ann/ANN.hpp"
#include "DataBlock.h"
#include "TextManipulation.h"
#include <sstream>

DNN::DNN()
{

}

DNN::DNN(const char* filename)
{

    define(filename);

}

DNN::~DNN()
{

    activations.clear();
    weights.clear();
    bias.clear();

}

DNN::DNN(const DNN & rhs)
{

}

void DNN::define(const char* filename)
{

    DataBlock data;
    data.load(filename);

    strvec layers = split(data("layers"),',');

    std::cout << "We have " << layers.size() << " layers\n";

    activations.resize(layers.size());
    weights.resize(layers.size()-1);
    bias.resize(layers.size()-1);

    unsigned int i;

    for(i=0;i<layers.size();++i)
    {

        unsigned int num_neurons = atof(layers[i].c_str());

        activations[i] = Matrix(num_neurons,1,0.0f);

        if(i > 0)
        {

            weights[i-1] = Matrix(activations[i].rows(),
                                  activations[i-1].rows(),
                                  1.0f);

            bias[i-1]    = Matrix(activations[i].rows(),1,1.0f);

        }

    }

    for(i=0;i<layers.size();++i)
    {
        std::cout << activations[i].rows() << " inputs in layer " << i << "\n";
    }

    for(i=0;i<layers.size()-1;++i)
    {
        std::cout << "Weight matrix dimensions: " << weights[i].rows() << " " << weights[i].cols();
        std::cout << " in layer " << i << "\n";
    }

    learningRate = atof(data("learning_rate").c_str());

}

void DNN::train(const Table& inputs, const Table& outputs)
{

    if(inputs.size() != outputs.size())
        throw std::runtime_error("Input rows must equal the output rows for training");

    unsigned int i;

    unsigned int L = activations.size() - 1;

    wGradient.resize(L);
    bGradient.resize(L);

    /*
     * Initialize our gradient descent corrections
     */
    for(i=0;i<L;++i)
    {

        wGradient.at(i) = Matrix(weights.at(i).rows(),weights.at(i).cols(),0.0f);
        bGradient.at(i) = Matrix(weights.at(i).rows(),1,0.0f);

    }

    /*
     * Backpropagate the mini batch
     */
    for(i=0;i<inputs.size();++i)
    {

        if(inputs.at(i).size() != activations.at(0).rows())
            throw std::runtime_error("Input size must equal the number of input neurons");

        backPropSet(inputs.at(i),outputs.at(i));

    }

    /*
     * Stochastic Gradient Descent
     */
    for(i=0;i<L;++i)
    {
        weights.at(i) = ( -learningRate / inputs.size() ) * wGradient.at(i);
        bias.at(i)    = ( -learningRate / inputs.size() ) * bGradient.at(i);
    }

    wGradient.clear();
    bGradient.clear();

}

dvec DNN::predict(const dvec & inputs)
{

    if(inputs.size() != activations.at(0).rows())
        throw std::runtime_error("Input size must equal the number of input neurons");

    feedForward(inputs);

    unsigned int layers = activations.size();

    return activations[layers-1].col_slice(0,0,activations[layers-1].rows()-1);

}

std::string DNN::toString()
{

    std::ostringstream oss;

    unsigned int i;

    for(i=0;i<weights.size();++i)
    {

        oss << "Layer " << i+1 << " weights:\n";

        oss << weights.at(i).to_string() << "\n";

        oss << "Layer " << i+1 << " bias:\n";

        oss << bias.at(i).to_string() << "\n";

    }

    return oss.str();

}

void DNN::backPropSet(const dvec& input,const dvec& output)
{

    unsigned int i;

    feedForward(input);

    unsigned int L = activations.size() - 1;

    /*
     * Start with the final layer
     */
    std::vector<Matrix> d(L+1);

    /*
     * Copy contents
     */
    Matrix out(output.size(),1);

    for(i=0;i<output.size();++i) out(i,0) = output.at(i);

    /*
     * Final layer error
     */
    Matrix DC = Matrix::apply(quad_dC_dA,activations.at(L),out);
    d.at(L)   = Matrix::had(DC,activations.at(L));

    /*
     * Backpropagate
     */
    for(i=L;i>0;--i)
    {

        Matrix wd = weights.at(i-1).T() * d.at(i);
        d.at(i-1)    = Matrix::had( wd, activations.at(i-1) );

    }

    /*
     * Calculate the gradient cost for this set
     */
    for(i=L;i>0;--i)
    {

        bGradient.at(i-1) = bGradient.at(i-1) + d.at(i);

        Matrix wg = d.at(i) * activations.at(i-1).T();
        wGradient.at(i-1) = wGradient.at(i-1) + wg;

    }

}

void DNN::feedForward(const dvec & inputs)
{

    unsigned int layers = activations.size();
    unsigned int i;

    for(i=0;i<inputs.size();++i)
    {
        activations.at(0)(i,0) = sigmoid(inputs.at(i));
    }

    for(i=1;i<layers;++i)
    {
        activations.at(i) = (weights.at(i-1)*activations.at(i-1) + bias.at(i-1)).apply(sigmoid);
    }

}
