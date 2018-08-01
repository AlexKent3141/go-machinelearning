#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "../tiny_dnn/tiny_dnn.h"
#include "../core/Board.h"
#include <string>
#include <vector>

using namespace tiny_dnn;
using namespace tiny_dnn::layers;

class Network
{
public:
    void Load(const std::string&);

    // Get the outputs for the specified board state.
    std::vector<double> GetOutput(const Board&);

private:
    network<sequential> _nn;

    vec_t GetInputs(const Board&) const;
};

#endif // __NETWORK_H__
