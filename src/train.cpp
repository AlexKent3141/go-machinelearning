#include "data/PrepareData.h"
#include "tiny_dnn/tiny_dnn.h"
#include <iostream>

#include <cassert>
#include <cstdlib>

using namespace tiny_dnn;
using namespace tiny_dnn::layers;

void Randomise(std::vector<Example>& examples)
{
    const int Seed = 314159265;
    std::srand(Seed);
    std::random_shuffle(examples.begin(), examples.end());
}

void ExampleSubset(const auto& exampleStart,
                   const auto& exampleEnd, /* Do not include this in the enumeration! */
                   std::vector<vec_t>& positions,
                   std::vector<label_t>& labels)
{
    auto it = exampleStart;
    while (it != exampleEnd)
    {
        // Copy the position and label into the supplied vectors.
        positions.push_back(it->first);
        labels.push_back(it->second);
        ++it;
    }
}

void ExampleSplit(const std::vector<Example>& examples,
                  std::vector<vec_t>& trainPositions,
                  std::vector<vec_t>& testPositions,
                  std::vector<label_t>& trainLabels,
                  std::vector<label_t>& testLabels,
                  double propForTraining)
{
    int lastTrainIndex = propForTraining * examples.size();
    ExampleSubset(examples.begin(),
                  examples.begin() + lastTrainIndex + 1,
                  trainPositions, trainLabels);

    ExampleSubset(examples.begin() + lastTrainIndex + 1,
                  examples.end(),
                  testPositions, testLabels);
}

int main()
{
    // Read in the example data to use for training & testing.
    auto examples = GetExamples("inputs.dat", "outputs.dat", 2);
    std::cout << "Num examples found: " << examples.size() << std::endl;
    assert(examples.size() > 0);

    Randomise(examples);
    std::cout << "Randomly ordered examples" << std::endl;

    std::vector<label_t> trainLabels, testLabels;
    std::vector<vec_t> trainPositions, testPositions;
    ExampleSplit(examples, trainPositions, testPositions, trainLabels, testLabels, 0.75);
    std::cout << "Split data into training and testing subsets" << std::endl;
    std::cout << "Training: " << trainPositions.size() << std::endl;
    std::cout << "Testing: " << testPositions.size() << std::endl;
    assert(trainPositions.size() == trainLabels.size());
    assert(testPositions.size() == testLabels.size());

    // Construct the network.
    network<sequential> net;

    net << conv(23, 23, 5, 3, 6) << tiny_dnn::activation::tanh()
        << fc(19*19*6, 361);

    return 0;
}
