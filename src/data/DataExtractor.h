#ifndef __DATA_EXTRACTOR_H__
#define __DATA_EXTRACTOR_H__

#include "SGFParser.h"
#include <fstream>
#include <string>

// This object extracts the data from a game of Go.
class DataExtractor
{
public:
    DataExtractor(const std::string& inputDataFile, const std::string& outputDataFile)
    {
    }

    void Generate(const std::vector<Move>& moves) const
    {
    }

private:
    std::ofstream inputs, outputs;
};

#endif // __DATA_EXTRACTOR_H__
