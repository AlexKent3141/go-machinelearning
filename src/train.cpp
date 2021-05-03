extern "C"
{
    #include "darknet/darknet.h"
    #include "darknet/data.h"
    #include "darknet/network.h"
    #include "darknet/parser.h"
}

#include "data/PrepareData.h"
#include "data/DataType.h"
#include <iostream>

#include <cassert>
#include <cstdlib>

const std::string& inputFile = "input";
const std::string& outputFile = "output";
const std::string& DataExt = ".dat";

const double ValueInitialError = 1000;

// The average proportion of points that are predicted correctly.
float matrix_territory_accuracy(matrix truth, matrix guess)
{
    const float SCALE = 180.5f;
    int i, j, correct = 0;
    float target, guessed;
    for (i = 0; i < truth.rows; ++i)
    {
        for (j = 0; j < truth.cols; j++)
        {
            // Get back to proportions.
            target = truth.vals[i][j] * SCALE;
            guessed = guess.vals[i][j] * SCALE;

            if ((target > 0.5f && guessed > 0.5f)
             || (target < 0.5f && guessed < 0.5f))
            {
                ++correct;
            }
        }
    }

    return correct / (361.0f * truth.rows);
}

float matrix_multi_topk_accuracy(matrix truth, matrix guess, int n)
{
    float top1 = matrix_topk_accuracy(truth, guess, 1);
    float topn = matrix_topk_accuracy(truth, guess, n);
    std::cout << "Top1: " << top1 << std::endl;
    std::cout << "TopN: " << topn << std::endl;
    return top1;
}

data GetGoData(int fileIndex, const std::string& dataFolder, DataType dataType)
{
    std::string inputFileName = dataFolder + "/" + inputFile + std::to_string(fileIndex) + DataExt;
    std::string outputFileName = dataFolder + "/" + outputFile + std::to_string(fileIndex) + DataExt;

    std::cout << "Loading data: " << inputFileName << " " << outputFileName << std::endl;

    data d = GetData(inputFileName, outputFileName, dataType);
    d.shallow = 0;

    std::cout << "Num examples found: " << d.X.rows << std::endl;
    std::cout << "Input columns: " << d.X.cols << std::endl;
    std::cout << "Output columns: " << d.y.cols << std::endl;
    assert(d.X.rows > 0);
    assert(d.X.rows == d.y.rows);

    randomize_data(d);
    std::cout << "Randomly ordered examples" << std::endl;

    return d;
}

// The arguments specify the relative folder and number of the data files.
int main(int argc, char** argv)
{
    srand(time(NULL));

    DataType dataType = Move;
    if (argc < 3)
    {
        std::cout << "Missing arguments." << std::endl;
        std::cout << "Require data folder path and maximum data file index." << std::endl;
        return -1;
    }

    if (argc < 4)
    {
        std::cout << "No data type specified: defaulting to move data" << std::endl;
    }
    else if (std::string(argv[3]) == "value")
    {
        dataType = Value;
    }
    else if (std::string(argv[3]) == "move_value")
    {
        dataType = MoveValue;
    }
    else if (std::string(argv[3]) == "territory")
    {
        dataType = Territory;
    }

    std::string dataFolder(argv[1]);
    int maxFileIndex = atoi(argv[2]);

    // Reserve the last data file for testing.
    data test = GetGoData(maxFileIndex, dataFolder, dataType);

    // Construct the network.
    network* net = load_network((char*)"net.cfg", NULL, 0);
  //network* net = load_network((char*)"net.cfg", (char*)"test_weights", 0);

    // Train.
    matrix guess = network_predict_data(*net, test);
    double bestAccuracy = dataType == Territory
        ? matrix_territory_accuracy(test.y, guess)
        : matrix_multi_topk_accuracy(test.y, guess, 5);

    std::cout << "Initial accuracy: " << bestAccuracy << std::endl;
    while ((int)get_current_batch(*net) < net->max_batches || net->max_batches == 0)
    {
        for (int t = 0; t < maxFileIndex; t++)
        {
            // Get the next training data set.
            data training = GetGoData(t, dataFolder, dataType);
            training.X.rows -= training.X.rows % net->batch;

            float loss = train_network(*net, training);
            std::cout << "Loss: " << loss << std::endl;

            free_data(training);

            // Test whether an improvement has happened.
            if (t % 10 == 0)
            {
                // Test and promote if it's an improvement.
                std::cout << "Testing..." << std::endl;

                matrix guess = network_predict_data(*net, test);

                float testAccuracy = dataType == Territory
                    ? matrix_territory_accuracy(test.y, guess)
                    : matrix_multi_topk_accuracy(test.y, guess, 5);

                std::cout << "Accuracy: " << testAccuracy << std::endl;

                if (testAccuracy > bestAccuracy)
                {
                    save_weights(*net, (char*)"test_weights");
                    std::cout << "Promoted" << std::endl;
                    bestAccuracy = testAccuracy;
                }
            }
        }
    }

    free_data(test);
    free_network(*net);

    return 0;
}
