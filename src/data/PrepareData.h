#ifndef __PREPARE_DATA_H__
#define __PREPARE_DATA_H__

extern "C"
{
    #include "../darknet/darknet.h"
}

#include <fstream>
#include <string>
#include <iostream>

// This file contains functions for parsing the extracted Go position data and preparing it
// to be used for training.

inline void AddPlane(int p,  float* in, const std::string& input, char featureType, int padding)
{
    // Add the padded plane for this feature type to the inputs.
    int paddedRowSize = 2*padding + 19;
    int paddedPlaneSize = paddedRowSize*paddedRowSize;

    for (int r = 0; r < 19; r++)
    {
        for (int c = 0; c < 19; c++)
        {
            bool isFeature = input[19*r+c] == featureType;
            in[paddedPlaneSize*p + (r+padding)*paddedRowSize + padding + c] = isFeature;
        }
    }
}

inline void GetRow(float* in, float* out, const std::string& input, size_t label, int padding)
{
    if (in != nullptr)
    {
        int p = 0;
        AddPlane(p++, in, input, 'P', padding);
        AddPlane(p++, in, input, 'O', padding);
        AddPlane(p++, in, input, '.', padding);
    }

    if (out != nullptr)
    {
        out[label] = 1;
    }
}
inline void GetRow(int i, matrix* in, matrix* out, const std::string& input, size_t label, int padding)
{
    GetRow(in->vals[i], out->vals[i], input, label, padding);
}

// Extract the examples from the specified files and apply the specified padding in x & y.
// Note: The padding is applied all the way around the board.
inline data GetData(const std::string& inputsFile, const std::string& labelsFile, int padding)
{
    std::ifstream inputs(inputsFile);
    std::ifstream labels(labelsFile);

    const int NumExamples = 276562;
    const int NumPlanes = 3;
    int paddedBoardArea = (2*padding + 19)*(2*padding + 19);
    matrix in = make_matrix(NumExamples, NumPlanes*paddedBoardArea);
    matrix out = make_matrix(NumExamples, 361);

    // The inputs and label for a specified example.
    int i = 0;
    if (inputs.is_open() && labels.is_open())
    {
        bool hasData = true;
        do
        {
            std::string input;
            std::getline(inputs, input);

            hasData = input.size() == 361;
            if (hasData)
            {
                std::string line;
                std::getline(labels, line);
                GetRow(i++, &in, &out, input, std::stoi(line), padding);
            }
        }
        while (hasData);
    }

    data d;
    d.X = in;
    d.y = out;

    inputs.close();
    labels.close();

    return d;
}

#endif // __PREPARE_DATA_H__
