#include "Network.h"
#include "../data/PrepareData.h"

void Network::Load(const std::string& filePath)
{
    _nn << conv(23, 23, 5, 3, 12)
        << tiny_dnn::activation::relu()
        << conv(19, 19, 3, 12, 12, padding::same)
        << tiny_dnn::activation::relu()
        << conv(19, 19, 3, 12, 12)
        << tiny_dnn::activation::relu()
        << fc(17*17*12, 19*19)
        << tiny_dnn::activation::softmax();

    _nn.load(filePath);
}

std::vector<double> Network::GetOutput(const Board& board)
{
    // Feed this board state through the network and capture the outputs.
    vec_t inputs = GetInputs(board);
    vec_t pred = _nn.predict(inputs);

    std::vector<double> outputs;
    for (float out : pred) outputs.push_back(out);

    return outputs;
}

vec_t Network::GetInputs(const Board& board) const
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

    return GetExample(rep, 0, 2).first;
}
