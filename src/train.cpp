extern "C"
{
    #include "darknet/darknet.h"
    #include "darknet/data.h"
}

#include "data/PrepareData.h"
#include <iostream>

#include <cassert>
#include <cstdlib>

int main()
{
    srand(time(NULL));

    // Read in the example data to use for training & testing.
    data d = GetData("inputs.dat", "outputs.dat", 0);
    std::cout << "Num examples found: " << d.X.rows << std::endl;
    std::cout << "Input columns: " << d.X.cols << std::endl;
    std::cout << "Output columns: " << d.y.cols << std::endl;
    assert(d.X.rows > 0);
    assert(d.X.rows == d.y.rows);

    randomize_data(d);
    std::cout << "Randomly ordered examples" << std::endl;

    // Split the data into training and testing subsets.
    data* split = split_data(d, 3, 4);
    std::cout << "Training examples: " << split[0].X.rows << std::endl;
    std::cout << "Testing examples: " << split[1].X.rows << std::endl;
    assert(split[0].X.rows > 0);
    assert(split[1].X.rows > 0);

    // Construct the network.
    network* net = load_network("net.cfg", "test_weights", 0);
    split[0].X.rows -= split[0].X.rows % net->batch;

    // Train.
    double bestAccuracy = network_accuracy(net, split[1]);
    std::cout << "Initial accuracy: " << bestAccuracy << std::endl;
    while (get_current_batch(net) < net->max_batches || net->max_batches == 0)
    {
        float loss = train_network(net, split[0]);
        std::cout << loss << std::endl;

        float testAccuracy = network_accuracy(net, split[1]);
        std::cout << "Test accuracy: " << testAccuracy << std::endl;
        if (testAccuracy > bestAccuracy)
        {
            save_weights(net, "test_weights");
            std::cout << "Promoted" << std::endl;
            bestAccuracy = testAccuracy;
        }
    }

    free_data(split[0]);
    free_data(split[1]);
    free(split);

    free_network(net);

    return 0;
}
