#ifndef __NETWORK_H__
#define __NETWORK_H__

extern "C"
{
    #include "../darknet/darknet.h"
}

#include "../core/Board.h"
#include <vector>

class Network
{
public:
    ~Network();

    void Load();

    // Get the outputs for the specified board state.
    std::vector<double> GetOutput(const Board&);

private:
    network* _nn = nullptr;

    void GetInputs(const Board&, float*) const;
};

#endif // __NETWORK_H__
