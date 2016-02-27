#include "ann/ANN.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <cstdlib>
#include <cmath>

ANN::ANN()
{

    std::string spec = "L:0:1";
    define(spec);
    networkSpec = spec;

}

ANN::ANN(std::string spec)
{

    define(spec);
    networkSpec = spec;

}

ANN::~ANN()
{

    neurons.clear();

}

ANN::ANN(const ANN & rhs)
{

    neurons = rhs.neurons;
    networkSpec = rhs.networkSpec;

    define(networkSpec);

}

void ANN::define(std::string spec)
{

    strvec cmds;

    cmds = splitString(spec,";\n");

	int i;
    for(i=0;i<cmds.size();++i)
    {
        executeCommand(cmds[i]);
    }

}

void ANN::train(const std::vector<dvec> & inputs,
                const std::vector<dvec> & outputs)
{

    /* Backpropagation algorithm here */

    zeroErrors(inputs.size());

    backPropagation(inputs,outputs);

    StochasticGradientDescent();

}

dvec ANN::predict(const dvec & inputs)
{

    /* Feedforward inputs here */

    initInputs(inputs);

    fireNeurons();

    return getOutputs();

}

void ANN::addLayer(const strvec& sections)
{

    if(sections.size() != 3)
        throw std::runtime_error("Layer definition requires 2 values");

    unsigned int layer       = atoi(sections[1].c_str());
    unsigned int num_neurons = atoi(sections[2].c_str());

    if(neurons.size() <= layer)
        neurons.resize(layer+1);

    if(neurons[layer].size() < num_neurons)
        neurons[layer].resize(num_neurons);

}

void ANN::addConnection(const strvec& sections)
{

    if(sections.size() != 5)
        throw std::runtime_error("Connection definition requires 4 values");

    unsigned int layer1  = atoi(sections[1].c_str());
    unsigned int neuron1 = atoi(sections[2].c_str());
    unsigned int layer2  = atoi(sections[3].c_str());
    unsigned int neuron2 = atoi(sections[4].c_str());

    Neuron* inputNeuron;

    try{

        inputNeuron = &(neurons.at(layer1).at(neuron1));

        neurons.at(layer2).at(neuron2).addInput(inputNeuron);

    }catch(std::exception& e){

        std::cerr << e.what() << "\n";
        throw std::runtime_error("Could not make neural connection");

    }

}

void ANN::executeCommand(std::string cmd)
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

        case 'N': /* Add a connection between two Nerons */

            addConnection(sections);

            break;

        default:

            throw std::runtime_error("Invalid network construction command");

            break;

    }

}

strvec ANN::splitString(std::string str, const char* delims)
{

    std::stringstream ss(str), sstok;
    std::string delims_str(delims), item;
    strvec tokens;
    char c;

    while(ss >> c) /* examine characters one at a time */
    {

        if(delims_str.find(c) == std::string::npos) sstok << c;

        else
        {

            sstok >> item;
            tokens.push_back(item);
            sstok.clear();
            item.clear();

        }

    }

    if(sstok >> item) tokens.push_back(item);

    return tokens;

}

void ANN::initInputs(const dvec & inputs)
{

    unsigned int i;

    for(i=0;i<neurons[0].size();++i)
    {
        neurons[0][i].addInput(&(inputs[i]));
    }

}

void ANN::fireNeurons()
{

    unsigned int i, j;

    for(i=0;i<neurons.size();++i)
    {
        for(j=0;j<neurons[i].size();++j)
        {
            neurons[i][j].setOutput();
        }
    }

}

dvec ANN::getOutputs()
{

    unsigned int i, L, numOutputs;

    L = neurons.size()-1;
    numOutputs = neurons[L].size();
    dvec result(numOutputs);

    for(i=0;i<numOutputs;++i)
    {
        result[i] = neurons[L][i].getOutput();
    }

    return result;

}

void ANN::zeroErrors(unsigned int trainSize)
{

    unsigned int i, j;
    for(i=0;i<neurons.size();++i)
    {
        for(j=0;j<neurons[i].size();++j)
        {
            neurons[i][j].initBackProp(trainSize);
        }
    }

}

/* Standard cost function derivative */
double ANN::quad_dC_dA(double a, double out)
{
    return a - out;
}

dvec ANN::apply(double (*func)(double,double), const dvec& vec1, const dvec& vec2)
{

    if(vec1.size() != vec2.size())
    {
        throw std::runtime_error("\"apply\" must be used with vectors of same size");
    }

    unsigned int i;
    dvec result(vec1.size());
    for(i=0;i<vec1.size();++i)
    {
        result[i] = func(vec1[i],vec2[i]);
    }

    return result;

}

void ANN::backPropagation(const std::vector<dvec> & inputs,
                          const std::vector<dvec> & outputs)
{

    unsigned int i;

    /* back propagate this training set */
    for(i=0;i<inputs.size();++i)
    {
        backPropSet(inputs[i],outputs[i],i);
    }

}

void ANN::backPropSet(const dvec& inputs, const dvec& outputsEx, unsigned int set)
{

    unsigned int i, j;

    dvec outputs = predict(inputs);

    for(i=0;i<neurons.size();++i)
    {
        for(j=0;j<neurons[i].size();++j)
        {
            neurons[i][j].saveOutput(set);
        }
    }

    /* 
     * In the future we can let the user pick a cost function
     * derivative to begin the backpropagation with
     */
    dvec dC_dA = apply(quad_dC_dA,outputs,outputsEx);
    unsigned int L = neurons.size()-1;

    for(i=0;i<neurons[L].size();++i)
    {
        neurons[L][i].setError(set, neurons[L][i].getOutput() * dC_dA[i] );
    }

    for(i=L;i>0;--i)
    {
        for(j=0;j<neurons[i].size();++j)
        {
            neurons[i][j].backProp(set);
        }
    }

}

void ANN::StochasticGradientDescent()
{

    unsigned int i, j;

    for(i=0;i<neurons.size();++i)
    {
        for(j=0;j<neurons[i].size();++j)
        {
            neurons[i][j].gradientDescentUpdate(5.0f);
        }
    }

}

/*
 * Now we define the Neuron class's
 * constructors and methods
 */

ANN::Neuron::Neuron()
{

    rawInput = NULL;
    bias = 0.0f;
    output = 0.0f;

}

ANN::Neuron::~Neuron()
{

    inputs.clear();
    weights.clear();

}

ANN::Neuron::Neuron(const Neuron & rhs)
{

    rawInput = NULL;
    weights = rhs.weights;
    bias = rhs.bias;
    output = 0.0f;

}

void ANN::Neuron::addInput(Neuron* input)
{

    inputs.push_back(input);
    if(inputs.size() > weights.size())
        weights.push_back(1.0f);

}

void ANN::Neuron::addInput(const double* input)
{

    rawInput = input;

}

void ANN::Neuron::setOutput()
{

    double z = 0.0f;

    if(rawInput) z += *rawInput;

    unsigned int i;
    for(i=0;i<inputs.size();++i)
    {
        z += weights[i] * inputs[i]->output;
    }

    output = activationFunction(z);

}

void ANN::Neuron::initBackProp(unsigned int trainSize)
{

    d_l.assign(trainSize,0.0f);
    sampleOutputs.assign(trainSize,0.0f);

}

void ANN::Neuron::backProp(unsigned int set)
{

    unsigned int i;
    for(i=0;i<inputs.size();++i)
    {

        Neuron* nextLayerInput = inputs[i];

        double d_lContrib  = d_l[set];
               d_lContrib *= weights[i];
               d_lContrib *= nextLayerInput->output;

        nextLayerInput->d_l[set] += d_lContrib;

    }

}

void ANN::Neuron::gradientDescentUpdate(double rate)
{

    unsigned int i, j;
    for(i=0;i<d_l.size();++i)
    {
        for(j=0;j<weights.size();++j)
        {
            weights[j] -= ( rate * (inputs[j]->sampleOutputs[i]) * d_l[i] ) / d_l.size();
        }

        bias -= ( rate * d_l[i] ) / d_l.size();

    }

}

/* sigmoid function */
double ANN::Neuron::activationFunction(double z)
{
    return 1.0f / (1.0f+exp(-1.0f*z));
}
