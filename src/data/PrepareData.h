#ifndef __PREPARE_DATA_H__
#define __PREPARE_DATA_H__

extern "C"
{
    #include "../darknet/darknet.h"
}

#include "DataType.h"
#include <cassert>
#include <fstream>
#include <string>
#include <iostream>

// This file contains functions for parsing the extracted Go position data and preparing it
// to be used for training.

inline void AddPlane(int p,  float* in, const std::string& input, char featureType)
{
    // Add the padded plane for this feature type to the inputs.
    int rowSize = 19;
    int planeSize = rowSize*rowSize;

    for (int r = 0; r < 19; r++)
    {
        for (int c = 0; c < 19; c++)
        {
            bool isFeature = input[19*r+c] == featureType;
            in[planeSize*p + r*rowSize + c] = isFeature;
        }
    }
}

inline void GetTerritoryOutput(float* out, const std::string& output)
{
    // To get back to probabilities multiply the outputs by 180.5.
    for (int i = 0; i < 361; i++)
    {
        char c = output[i];
        out[i] = c == 'P' ? 1 : c == 'O' ? 0 : 0.5;
        out[i] /= 180.5f;
    }
}

inline void GetRow(float* in, float* out, const std::string& input, const std::string& output, DataType dataType)
{
    if (in != nullptr)
    {
        int p = 0;
        AddPlane(p++, in, input, 'P');
        AddPlane(p++, in, input, 'O');
        AddPlane(p++, in, input, '.');
    }

    if (out != nullptr)
    {
        switch (dataType)
        {
            case Territory:
                // Output the territory distribution. This is just one plane.
                GetTerritoryOutput(out, output);
                break;
            case Move:
                // Just turn on the move location.
                out[stoi(output)] = 1;
                break;
            case Value:
                // There are two outputs which encode the probability of each player winning.
                out[0] = output[0] - 48;
                out[1] = 49 - output[0];
                break;
            case MoveValue:
                // The output consists of the move location and win/loss separated by a space.
                std::string loc = output.substr(0, output.size()-2);
                out[stoi(loc)] = 1;
                out[361] = output.back() == '1';
                break;
        }
    }
}

inline void GetRow(int i, matrix* in, matrix* out, const std::string& input, const std::string& output, DataType dataType)
{
    GetRow(in->vals[i], out->vals[i], input, output, dataType);
}

// Extract the examples from the specified files and apply the specified padding in x & y.
// Note: The padding is applied all the way around the board.
inline data GetData(const std::string& inputsFile, const std::string& labelsFile, DataType dataType)
{
    // Initially find the number of lines in the file.
    std::string l;
    int n = 0;
    std::ifstream test(inputsFile);
    while (std::getline(test, l)) ++n;
    test.close();

    std::ifstream inputs(inputsFile);
    std::ifstream labels(labelsFile);

    const int NumPlanes = 3;
    int boardArea = 19*19;
    int outputSize = dataType == MoveValue ? boardArea+1
                   : dataType == Move ? boardArea
                   : dataType == Value ? 2
                   : boardArea;

    matrix in = make_matrix(n, NumPlanes*boardArea);
    matrix out = make_matrix(n, outputSize);

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
                std::string output;
                std::getline(labels, output);
                assert(!output.empty());
                GetRow(i++, &in, &out, input, output, dataType);
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
