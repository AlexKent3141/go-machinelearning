#include "Network.h"
#include "../data/PrepareData.h"
#include "../data/DataType.h"
#include <iostream>

Network::~Network()
{
    if (_nn != nullptr)
    {
        free_network(_nn);
        _nn = nullptr;
    }
}

void Network::Load(const char* net, const char* weights)
{
    _nn = load_network((char*)net, (char*)weights, 0);
}

std::vector<double> Network::GetOutput(const Board& board)
{
    // Feed this board state through the network and capture the outputs.
    float* inputs = new float[3*361];
    GetInputs(board, inputs);
    float* pred = network_predict(_nn, inputs);

    std::vector<double> outputs;
    for (int i = 0; i < 361; i++) outputs.push_back(pred[i]);

    delete[] inputs;

    std::cout << "Win prediction: " << pred[361] << std::endl;

    return outputs;
}

void Network::GetInputs(const Board& board, float* inputs) const
{
    // First convert to the expected string representation then create the feature planes.
    std::string rep;
    Colour colourToMove = board.ColourToMove();
    for (int r = 0; r < 19; r++)
    {
        std::string row;
        for (int c = 0; c < 19; c++)
        {
            Colour col = board.PointColour(19*r+c);
            row += col == colourToMove ? 'P' : col != None ? 'O' : '.';
        }

        rep += row;
    }

    GetRow(inputs, nullptr, rep, "", MoveValue);
}
