#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <vector>

class NeuralCuda
{
public:
    NeuralCuda(const std::vector<int> &layerSizes, int cars);
    ~NeuralCuda();

    void SetInputs(const std::vector<float> &inputs);
    void SetWeights(const std::vector<float> &weights);
    void FeedForward(std::vector<float> &outputVec);

private:
    float *d_inputs,
        *d_weights,
        *d_outputs;
    int *d_layerSizes;
    int numLayers;
    int inputSize;
    int outputSize;
    int totalWeights;
    std::vector<int> layerSizes;
    int amountOfCars;

    void AllocateMemory();
    void FreeMemory();
};

#endif // NEURAL_NETWORK_H
