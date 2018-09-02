#include "data/DataExtractor.h"
#include "data/MoveDataExtractor.h"
#include "data/ValueDataExtractor.h"
#include "data/SGFParser.h"
#include "dirent.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

void GetFilesByExtension(const std::string& dirPath, const std::string& ext, int earliestYear, std::vector<std::string>& fileNames)
{
    const size_t extLen = ext.size();
    DIR* dpdf = opendir(dirPath.c_str());
    if (dpdf != nullptr)
    {
        struct dirent* epdf;
        while ((epdf = readdir(dpdf)) != nullptr)
        {
            auto fileName = std::string(epdf->d_name);
            if (fileName.size() >= extLen && fileName.substr(fileName.size() - extLen) == ext)
            {
                // Check the year.
                if (stoi(fileName.substr(0, 4)) >= earliestYear)
                {
                    fileNames.push_back(dirPath + "/" + fileName);
                }
            }
            else if (epdf->d_type == DT_DIR)
            {
                if (epdf->d_name[0] != '.')
                {
                    GetFilesByExtension(dirPath + fileName, ext, earliestYear, fileNames);
                }
            }
        }
    }

    closedir(dpdf);
}

// Recursively search the directory and find all sgf files for games that happened after the
// specified year.
std::vector<std::string> GetGameFiles(const std::string& dirPath, int earliestYear)
{
    std::vector<std::string> files;
    GetFilesByExtension(dirPath, "sgf", earliestYear, files);
    return files;
}

int main(int argc, char** argv)
{
    bool valueData = false;
    if (argc < 2)
    {
        std::cout << "No games folder specified" << std::endl;
        return -1;
    }

    if (argc < 3)
    {
        std::cout << "No data type specified: defaulting to move data" << std::endl;
    }
    else
    {
        valueData = std::string(argv[2]) == "value";
    }

    auto dir = std::string(argv[1]);
    auto fileNames = GetGameFiles(dir, 2000);

    SGFParser parser;
    DataExtractor* extractor = valueData
        ? (DataExtractor*)new ValueDataExtractor("go_data", fileNames.size())
        : (DataExtractor*)new MoveDataExtractor("go_data", fileNames.size());

    for (size_t i = 0; fileNames.size(); i++)
    {
        std::string file = fileNames[i];
        std::cout << "Parsing file " << i << "/" << fileNames.size() << " " << file << std::endl;

        if (parser.Parse(file))
        {
            extractor->Generate(parser.Moves(), parser.Result(), file);
        }
        else
        {
            std::cout << "Skipping invalid file" << std::endl;
        }
    }

    delete extractor;

    return 0;
}
