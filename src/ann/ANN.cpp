#include "ann/ANN.hpp"

ANN::ANN()
{

}

ANN::~ANN()
{

}

ANN::ANN(const ANN & rhs)
{

}

/* Standard cost function derivative */
double ANN::quad_dC_dA(double a, double out)
{

    return a - out;

}

double ANN::sigmoid(double z)
{

    return 1.0f / (1.0f+exp(-1.0f*z));

}
