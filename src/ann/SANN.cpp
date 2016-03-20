#include "ann/ANN.hpp"

SANN::SANN()
{

    std::string spec = "L:0:1";
    define(spec);
    networkSpec = spec;

}

SANN::SANN(std::string spec)
{

    define(spec);
    networkSpec = spec;

}

SANN::~SANN()
{

    activations.clear();
    weights.clear();

}

SANN::SANN(const SANN & rhs)
{

    networkSpec = rhs.networkSpec;
    define(networkSpec);

}

void SANN::train(const std::vector<dvec> & inputs,
                 const std::vector<dvec> & outputs)
{

    if(inputs[0].size() != activations.at(0).rows())
        throw std::runtime_error("Input size must equal the number of input neurons");

    feedForward(inputs[0]);

    unsigned int L = activations.size() - 1;
    unsigned int i;
    std::vector<Matrix> d(L+1);

    Matrix out(outputs[0].size(),1);
    for(i=0;i<outputs[0].size();++i)
    {
        out(i,0) = outputs[0][i];
    }

    Matrix DC = Matrix::apply(quad_dC_dA,activations[L],out);

    d[L] = Matrix::had(DC,activations[L]);

    for(i=L;i>0;--i)
    {
        Matrix wd = weights[i-1].T()*d[i];
        d[i-1] = Matrix::had( wd, activations[i-1] );
    }

    for(i=0;i<d.size();++i)
    {
        std::cout << d[i].to_string() << "\n";
    }

}

dvec SANN::predict(const dvec & inputs)
{

    if(inputs.size() != activations.at(0).rows())
        throw std::runtime_error("Input size must equal the number of input neurons");

    feedForward(inputs);

    unsigned int layers = activations.size();

    return activations[layers-1].col_slice(0,0,activations[layers-1].rows()-1);

}

void SANN::feedForward(const dvec & inputs)
{

    unsigned int layers = activations.size();
    unsigned int i;

    for(i=0;i<inputs.size();++i)
    {
        activations[0](i,0) = sigmoid(inputs[i]);
    }

    for(i=1;i<layers;++i)
    {
        activations[i] = (weights[i-1]*activations[i-1] + bias[i-1]).apply(sigmoid);
    }

}

void SANN::addLayer(const strvec& sections)
{

    if(sections.size() != 3)
        throw std::runtime_error("Layer definition requires 2 values");

    unsigned int layer       = atoi(sections[1].c_str());
    unsigned int num_neurons = atoi(sections[2].c_str());

    if(layer != activations.size() )
        throw std::runtime_error("Layer additions must be sequential");

    activations.push_back( Matrix(num_neurons,1,0.0f) );

    if(layer > 0)
    {
        weights.push_back( Matrix(activations[layer  ].rows(),
                                  activations[layer-1].rows(),
                                  1.0f) );

        bias.push_back( Matrix(activations[layer].rows(),1,1.0f) );
    }

}

void SANN::executeCommand(std::string cmd)
{

    strvec sections;

    sections = splitString(cmd,":");

    /*
     * The first letter will tell us
     * what to do with the next values
     */
    char type = sections[0][0];

    switch(type)
    {

        case 'L': /* Add a layer of Neurons */

            addLayer(sections);

            break;

        default:

            throw std::runtime_error("Invalid network construction command");

            break;

    }

}
