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

typedef std::vector<double> dvec;
typedef std::vector<std::string> strvec;

/*
 * Base class for all neural network
 * implmentations in free-time applications
 */
class BasicANN {

    public:
        BasicANN();
        ~BasicANN();
        BasicANN(const BasicANN & rhs);

        void define(std::string spec);

        virtual void train(const std::vector<dvec> & inputs,
                           const std::vector<dvec> & outputs) =0;

        virtual dvec predict(const dvec & inputs) =0;

    protected:

        std::string networkSpec;

        virtual void executeCommand(std::string cmd) =0;

        static strvec splitString(std::string str, const char* delims);

        static double sigmoid(double z);

        static double quad_dC_dA(double a, double out);

};

/*
 * Artifical Neural Network
 */
class ANN : public BasicANN {

    public:

        ANN();
        ANN(std::string spec);
        ~ANN();
        ANN(const ANN & rhs);

        void train(const std::vector<dvec> & inputs,
                   const std::vector<dvec> & outputs);

        dvec predict(const dvec & inputs);

    private:

        /*
         * A nested Neuron class will give the
         * opportunity to make nice interfaces
         * when running the back propagation
         */
        class Neuron {

            public:

                Neuron();
                ~Neuron();
                Neuron(const Neuron & rhs);

                void addInput(Neuron* inputNeuron);
                void addInput(const double* rawInput);

                double getOutput() const { return output; }

                void setOutput();

                void saveOutput(unsigned int set) { sampleOutputs[set] = output; }

                void setError(unsigned int set, double value) { d_l[set] = value; }

                void initBackProp(unsigned int trainSize);

                void backProp(unsigned int trainSet);

                void gradientDescentUpdate(double rate);

            private:

                /* for mini batch learning */
                dvec d_l;
                dvec sampleOutputs;
                /***************************/

                std::vector<Neuron*> inputs;
                const double* rawInput;

                dvec weights;

                double bias;

                double output;

                double activationFunction(double z);

        };

        /* This is the network */
        std::vector< std::vector<Neuron> > neurons;

        /* Methods for building the ANN */
        void addLayer(const strvec& sections);

        void addConnection(const strvec& sections);

        void executeCommand(std::string cmd);

        /* Methods for feedforward calculation */

        void initInputs(const dvec & inputs);

        void fireNeurons();

        dvec getOutputs();

        /* Methods for backpropagation and shochastic gradient descent */

        void zeroErrors(unsigned int trainSize);

        dvec apply(double (*func)(double,double), const dvec& vec1, const dvec& vec2);

        void backPropagation(const std::vector<dvec> & inputs,
                             const std::vector<dvec> & outputs);

        void backPropSet(const dvec& inputs, const dvec& outputs, unsigned int set);

        void StochasticGradientDescent();

};

/*
 * Saturated Artifical Neural Network:
 * All neurons feedforward output to
 * all other neurons in the next layer
 */
class SANN : public BasicANN {

    public:

        SANN();
        SANN(std::string spec);
        ~SANN();
        SANN(const SANN & rhs);

        void train(const std::vector<dvec> & inputs,
                   const std::vector<dvec> & outputs);

        dvec predict(const dvec & inputs);

    private:

        /* This is the network */
        std::vector<Matrix> activations;
        std::vector<Matrix> weights;
        std::vector<Matrix> bias;

        /* Methods for building the ANN */
        void addLayer(const strvec& sections);

        void executeCommand(std::string cmd);

        void feedForward(const dvec & inputs);

};

#endif
