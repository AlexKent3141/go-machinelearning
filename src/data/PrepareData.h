#ifndef __PREPARE_DATA_H__
#define __PREPARE_DATA_H__

#include "../tiny_dnn/tiny_dnn.h"

// This file contains functions for parsing the extracted Go position data and preparing it
// to be used for training.

// A training example consists of the inputs and the known label.
typedef std::pair<tiny_dnn::vec_t, tiny_dnn::label_t> Example;

inline void AddEmptyRow(tiny_dnn::vec_t& inputs, int padding)
{
    for (int i = 0; i < 2*padding+19; i++) inputs.push_back(0);
}

inline void AddRow(tiny_dnn::vec_t& inputs, const std::string& row, char featureType, int padding)
{
    for (int i = 0; i < padding; i++) inputs.push_back(0);
    for (int i = 0; i < 19; i++) inputs.push_back(row[i] == featureType);
    for (int i = 0; i < padding; i++) inputs.push_back(0);
}

inline void AddPlane(tiny_dnn::vec_t& inputs, const std::string& input, char featureType, int padding)
{
    // Add the padded plane for this feature type to the inputs.
    for (int i = 0; i < padding; i++) AddEmptyRow(inputs, padding);
    for (int i = 0; i < 19; i++) AddRow(inputs, input.substr(19*i, 19), featureType, padding);
    for (int i = 0; i < padding; i++) AddEmptyRow(inputs, padding);
}

inline Example GetExample(const std::string& input, size_t label, int padding)
{
    tiny_dnn::vec_t inputs;
    AddPlane(inputs, input, 'P', padding);
    AddPlane(inputs, input, 'O', padding);
    AddPlane(inputs, input, '.', padding);

    return { inputs, label };
}

// Extract the examples from the specified files and apply the specified padding in x & y.
// Note: The padding is applied all the way around the board.
inline std::vector<Example> GetExamples(const std::string& inputsFile, const std::string& labelsFile, int padding)
{
    std::vector<Example> examples;
    std::ifstream inputs(inputsFile);
    std::ifstream labels(labelsFile);

    // The inputs and label for a specified example.
    if (inputs.is_open() && labels.is_open())
    {
        bool hasExamples = true;
        do
        {
            std::string input;
            std::getline(inputs, input);

            hasExamples = input.size() == 361;
            if (hasExamples)
            {
                std::string line;
                std::getline(labels, line);
                examples.push_back(GetExample(input, std::stoi(line), padding));
            }
        }
        while (hasExamples);
    }

    inputs.close();
    labels.close();

    return examples;
}

#endif // __PREPARE_DATA_H__
