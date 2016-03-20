#include "ann/ANN.hpp"

BasicANN::BasicANN()
{

}

BasicANN::~BasicANN()
{

}

BasicANN::BasicANN(const BasicANN & rhs)
{

}

void BasicANN::define(std::string spec)
{

    strvec cmds;

    cmds = splitString(spec,";\n");

	int i;
    for(i=0;i<cmds.size();++i)
    {
        executeCommand(cmds[i]);
    }

}

strvec BasicANN::splitString(std::string str, const char* delims)
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

/* Standard cost function derivative */
double BasicANN::quad_dC_dA(double a, double out)
{
    return a - out;
}

double BasicANN::sigmoid(double z)
{
    return 1.0f / (1.0f+exp(-1.0f*z));
}
