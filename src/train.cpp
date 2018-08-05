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

    // Construct the network and specify the optimiser to use.
    network<sequential> nn;
    adagrad optimiser;
    optimiser.alpha = 0.001;

    nn << conv(23, 23, 5, 3, 12)
       << tiny_dnn::activation::relu()
       << conv(19, 19, 3, 12, 12, padding::same)
       << tiny_dnn::activation::relu()
       << conv(19, 19, 3, 12, 12)
       << tiny_dnn::activation::relu()
       << fc(17*17*12, 19*19)
       << tiny_dnn::activation::softmax();

  //nn.load("go-model");

  //tiny_dnn::result res = nn.test(testPositions, testLabels);
  //std::cout << res.num_success << "/" << res.num_total << std::endl;

  //return 0;

    // Setup a callback to report progress and begin training.
    // This is taken verbatim from the tiny-dnn mnist example.
    tiny_dnn::progress_display disp(trainPositions.size());
    tiny_dnn::timer t;
    int minibatchSize = 10;
    int numEpochs     = 1000;

    optimiser.alpha *= static_cast<tiny_dnn::float_t>(std::sqrt(minibatchSize));

    double bestRatio = 0;

    // create callback
    auto on_enumerate_epoch = [&]() {
        std::cout << t.elapsed() << "s elapsed." << std::endl;
        tiny_dnn::result res = nn.test(testPositions, testLabels);
        std::cout << res.num_success << "/" << res.num_total << std::endl;

        double ratio = (double)res.num_success / res.num_total;
        std::cout << ratio << " " << bestRatio << std::endl;
        if (ratio > bestRatio)
        {
            std::cout << "Promoted" << std::endl;
            nn.save("go-model");
            bestRatio = ratio;
        }

        disp.restart(trainPositions.size());
        t.restart();
    };

    auto on_enumerate_minibatch = [&]() { disp += minibatchSize; };

    // training
    nn.train<tiny_dnn::cross_entropy_multiclass>(
        optimiser, trainPositions, trainLabels, minibatchSize,
        numEpochs, on_enumerate_minibatch,
        on_enumerate_epoch);

    std::cout << "Training complete" << std::endl;

    // test and show results
    nn.test(testPositions, testLabels).print_detail(std::cout);

    // save nnwork model & trained weights
    nn.save("go-model");

    return 0;
}
