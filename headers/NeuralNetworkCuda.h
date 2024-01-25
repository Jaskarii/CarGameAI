// NeuralNetworkCUDA.h
#ifndef NEURAL_NETWORK_CUDA_H
#define NEURAL_NETWORK_CUDA_H

#include <vector>
#include <random>
#include <cuda_runtime.h>

class NeuralNetworkCUDA
{
public:
    NeuralNetworkCUDA(const std::vector<int> &layers);
    ~NeuralNetworkCUDA();

    void CopyWeights(const NeuralNetworkCUDA &copyFrom);
    std::vector<float> FeedForward(const std::vector<float> &inputs);
    void Mutate(float mutationRate, float mutationScale);
    void AddFitness(float fit);
    void SetFitness(float fit);
    float GetFitness() const;
    void PrintWeights() const;
    int CompareTo(const NeuralNetworkCUDA &other) const;

private:
    std::vector<int> layers;
    float *d_neurons;                                          // Pointer for neurons on the device (GPU)
    float *d_weights;                                          // Pointer for weights on the device (GPU)
    std::vector<std::vector<float>> host_neurons;              // Host-side representation of neurons
    std::vector<std::vector<std::vector<float>>> host_weights; // Host-side representation of weights
    float fitness;

    void InitNeurons();
    void InitWeights();
    static float RandomWeight(); // Function to generate a random weight

    // Helper methods for CUDA memory management
    void AllocateDeviceMemory();
    void FreeDeviceMemory();
    void CopyToDevice();
    void CopyToHost();
};

#endif // NEURAL_NETWORK_CUDA_H
