#ifndef __DATA_EXTRACTOR_H__
#define __DATA_EXTRACTOR_H__

#include "../core/Move.h"
#include <cmath>
#include <vector>
#include <fstream>

class DataExtractor
{
public:
    DataExtractor() = delete;

    DataExtractor(const std::string& targetDir, int numGames)
    {
        // The data extractor potentially needs to split the data over multiple files (because
        // there may not be enough RAM available to load all of the data at once when training).
        const int PositionsPerFile = 100000;
        const int AverageMovesPerGame = 200;
        int numFilesRequired = ceil((8*AverageMovesPerGame*(double)numGames) / PositionsPerFile);

        const std::string& InputFile = "input";
        const std::string& OutputFile = "output";
        const std::string& DataExt = ".dat";
        for (int i = 0; i < numFilesRequired; i++)
        {
            _inputs.push_back(std::ofstream(targetDir + "/" + InputFile + std::to_string(i) + DataExt));
            _outputs.push_back(std::ofstream(targetDir + "/" + OutputFile + std::to_string(i) + DataExt));
        }
    }

    virtual ~DataExtractor()
    {
        for (size_t i = 0; i < _inputs.size(); i++)
        {
            _inputs[i].close();
            _outputs[i].close();
        }
    }

    // This method actually generates the data based on the game details.
    virtual void Generate(const std::vector<Move>& moves, const std::string& result, const std::string& sgfFilePath) = 0;

protected:
    std::vector<std::ofstream> _inputs;
    std::vector<std::ofstream> _outputs;

    std::string Flip(const std::string& input) const
    {
        std::string flipped(input);
        for (int r = 0; r < 19; r++)
        {
            for (int c = 0; c < 19; c++)
            {
                int i = 19*r + c;
                flipped[i] = input[Flip(i)];
            }
        }

        return flipped;
    }

    int Flip(int loc) const
    {
        int x = loc % 19;
        int y = loc / 19;
        return 19*y + (18 - x);
    }

    std::string Rotate(const std::string& input) const
    {
        std::string rotated(input);
        for (int r = 0; r < 19; r++)
        {
            for (int c = 0; c < 19; c++)
            {
                int i = 19*r + c;
                rotated[i] = input[Rotate(i)];
            }
        }

        return rotated;
    }

    int Rotate(int loc) const
    {
        int x = loc % 19;
        int y = loc / 19;
        return 19*(18-x) + y;
    }
};

#endif // __DATA_EXTRACTOR_H__
