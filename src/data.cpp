#include "data/DataExtractor.h"
#include "data/SGFParser.h"
#include "dirent.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

std::vector<std::string> GetFilesByExtension(const std::string& dirPath, const std::string& ext)
{
    std::vector<std::string> fileNames;
    const size_t extLen = ext.size();
    DIR* dpdf = opendir(dirPath.c_str());
    if (dpdf != nullptr)
    {
        struct dirent* epdf;
        while ((epdf = readdir(dpdf)) != nullptr)
        {
            auto fileName = std::string(epdf->d_name);
            if (fileName.size() >= extLen && fileName.substr(fileName.size() - extLen) == ext)
                fileNames.push_back(std::string(epdf->d_name));
        }
    }

    closedir(dpdf);

    return fileNames;
}

int main()
{
    const std::string& dirPath = "../../../GoRecords/2005/";
    auto fileNames = GetFilesByExtension(dirPath, "sgf");

    SGFParser parser;
    for (auto& file : fileNames)
    {
        std::string filePath = dirPath + file;
        std::cout << "Parsing file: " << filePath << std::endl;

        // TODO
        parser.Parse(filePath);
    }

    return 0;
}
