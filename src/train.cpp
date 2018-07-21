#include "data/PrepareData.h"
#include <iostream>

int main()
{
    // Read in the example data to use for training & testing.
    auto examples = GetExamples("inputs.dat", "outputs.dat", 2);
    std::cout << "Num examples found: " << examples.size() << std::endl;

    return 0;
}
