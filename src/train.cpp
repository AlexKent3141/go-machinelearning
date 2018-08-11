extern "C"
{
    #include "darknet/darknet.h"
    #include "darknet/data.h"
}

#include "data/PrepareData.h"
#include <iostream>

#include <cassert>
#include <cstdlib>

const std::string& inputFile = "input";
const std::string& outputFile = "output";
const std::string& DataExt = ".dat";

data GetGoData(int fileIndex, const std::string& dataFolder)
{
    std::string inputFileName = dataFolder + "/" + inputFile + std::to_string(fileIndex) + DataExt;
    std::string outputFileName = dataFolder + "/" + outputFile + std::to_string(fileIndex) + DataExt;

    std::cout << "Loading data: " << inputFileName << " " << outputFileName << std::endl;

    data d = GetData(inputFileName, outputFileName);
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

    if (argc < 3)
    {
        std::cout << "Missing arguments." << std::endl;
        std::cout << "Require data folder path and maximum data file index." << std::endl;
        return -1;
    }

    std::string dataFolder(argv[1]);
    int maxFileIndex = atoi(argv[2]);

    // Reserve the last data file for testing.
    data test = GetGoData(maxFileIndex, dataFolder);

    // Construct the network.
    network* net = load_network("net.cfg", "test_weights", 0);

    // Train.
    double bestAccuracy = network_accuracy(net, test);
    std::cout << "Initial accuracy: " << bestAccuracy << std::endl;
    while (get_current_batch(net) < net->max_batches || net->max_batches == 0)
    {
        for (int t = 0; t < maxFileIndex; t++)
        {
            // Get the next training data set.
            data training = GetGoData(t, dataFolder);
            training.X.rows -= training.X.rows % net->batch;

            float loss = train_network(net, training);
            std::cout << loss << std::endl;

            free_data(training);

            // Test whether an improvement has happened.
            if (t % 10 == 0)
            {
                // Test and promote if it's an improvement.
                std::cout << "Testing..." << std::endl;
                float testAccuracy = network_accuracy(net, test);
                std::cout << "Test accuracy: " << testAccuracy << std::endl;
                if (testAccuracy > bestAccuracy)
                {
                    save_weights(net, "test_weights");
                    std::cout << "Promoted" << std::endl;
                    bestAccuracy = testAccuracy;
                }
            }
        }
    }

    free_data(test);
    free_network(net);

    return 0;
}
