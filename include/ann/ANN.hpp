#ifndef ANN_HPP
#define ANN_HPP

#include <vector>
#include <string>

typedef std::vector<double> dvec;
typedef std::vector<std::string> strvec;

/*
 * Artifical Neural Network
 */
class ANN{

    public:

        ANN();
        ANN(std::string spec);
        ~ANN();
        ANN(const ANN & rhs);

        void define(std::string spec);

        void train(const std::vector<dvec> & inputs,
                   const std::vector<dvec> & outputs);

        dvec predict(const dvec & inputs);

    private:

        /*
         * A nested Neuron class will give the
         * opportunity to make nice interfaces
         * when running the back propagation
         */
        class Neuron{

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

        std::string networkSpec;

        /* Methods for building the ANN */
        void addLayer(const strvec& sections);

        void addConnection(const strvec& sections);

        void executeCommand(std::string cmd);

        static strvec splitString(std::string str, const char* delims);

        /* Methods for feedforward calculation */

        void initInputs(const dvec & inputs);

        void fireNeurons();

        dvec getOutputs();

        /* Methods for backpropagation and shochastic gradient descent */

        void zeroErrors(unsigned int trainSize);

        static double quad_dC_dA(double a, double out);

        dvec apply(double (*func)(double,double), const dvec& vec1, const dvec& vec2);

        void backPropagation(const std::vector<dvec> & inputs,
                             const std::vector<dvec> & outputs);

        void backPropSet(const dvec& inputs, const dvec& outputs, unsigned int set);

        void StochasticGradientDescent();

};

#endif
