#include "NeuralNetworkCuda.h"
#include "CudaKernels.cuh"

NeuralNetworkCUDA::NeuralNetworkCUDA(const std::vector<int> &layers)
    : layers(layers), d_neurons(nullptr), d_weights(nullptr), fitness(0)
{
    AllocateDeviceMemory();
    InitNeurons();
    InitWeights();
}

NeuralNetworkCUDA::~NeuralNetworkCUDA()
{
    FreeDeviceMemory();
}

void NeuralNetworkCUDA::AllocateDeviceMemory()
{
    // Calculate total size for neurons and weights
    size_t totalNeuronsSize = 0;
    size_t totalWeightsSize = 0;
    for (size_t i = 0; i < layers.size(); ++i)
    {
        totalNeuronsSize += layers[i];
        if (i > 0)
        {
            totalWeightsSize += layers[i] * layers[i - 1];
        }
    }

    // Allocate memory on the device
    cudaMalloc(&d_neurons, totalNeuronsSize * sizeof(float));
    cudaMalloc(&d_weights, totalWeightsSize * sizeof(float));
}

void NeuralNetworkCUDA::FreeDeviceMemory()
{
    cudaFree(d_neurons);
    cudaFree(d_weights);
}

void NeuralNetworkCUDA::CopyToDevice()
{
    size_t offset = 0;
    for (size_t i = 0; i < layers.size(); ++i)
    {
        size_t layerSize = layers[i];

        // Copy neurons for each layer
        cudaMemcpy(d_neurons + offset, host_neurons[i].data(), layerSize * sizeof(float), cudaMemcpyHostToDevice);
        offset += layerSize;

        if (i > 0)
        {
            // Copy weights for each layer (excluding the input layer)
            size_t weightsSize = layers[i] * layers[i - 1];
            cudaMemcpy(d_weights + offset, host_weights[i - 1].data(), weightsSize * sizeof(float), cudaMemcpyHostToDevice);
            offset += weightsSize;
        }
    }
}

void NeuralNetworkCUDA::CopyToHost()
{
    // Example: Assuming you have a host-side representation of neurons
    size_t offset = 0;
    for (size_t i = 0; i < layers.size(); ++i)
    {
        size_t layerSize = layers[i];

        // Copy neurons for each layer
        cudaMemcpy(host_neurons[i].data(), d_neurons + offset, layerSize * sizeof(float), cudaMemcpyDeviceToHost);
        offset += layerSize;

        // If you need to copy weights back to host, do it similarly
        // ...
    }
}

void NeuralNetworkCUDA::InitNeurons()
{
    size_t totalNeuronsSize = 0;
    for (int layerSize : layers)
    {
        totalNeuronsSize += layerSize;
    }

    // Initialize neuron values to zero
    cudaMemset(d_neurons, 0, totalNeuronsSize * sizeof(float));
}

void NeuralNetworkCUDA::InitWeights()
{
    size_t totalWeightsSize = 0;
    for (size_t i = 1; i < layers.size(); ++i)
    {
        totalWeightsSize += layers[i] * layers[i - 1];
    }

    // Host vector for weights
    std::vector<float> hostWeights(totalWeightsSize);

    // Initialize weights on the host
    for (float &weight : hostWeights)
    {
        weight = RandomWeight(); // Your method that returns a random float
    }

    // Copy initialized weights to the device
    cudaMemcpy(d_weights, hostWeights.data(), totalWeightsSize * sizeof(float), cudaMemcpyHostToDevice);
}

void NeuralNetworkCUDA::CopyWeights(const NeuralNetworkCUDA &copyFrom)
{
    // First, calculate the total size of all weights
    size_t totalWeightsSize = 0;
    for (size_t i = 1; i < layers.size(); ++i)
    {
        totalWeightsSize += layers[i] * layers[i - 1];
    }

    // Copy weights from one GPU memory space to another
    cudaMemcpy(d_weights, copyFrom.d_weights, totalWeightsSize * sizeof(float), cudaMemcpyDeviceToDevice);
}

std::vector<float> NeuralNetworkCUDA::FeedForward(const std::vector<float> &inputs)
{
    // Copy inputs to the device
    cudaMemcpy(d_neurons, inputs.data(), inputs.size() * sizeof(float), cudaMemcpyHostToDevice);
    float *d_input = d_neurons; // Initially, input is the first layer's neurons
    float *d_output;

    for (size_t i = 1; i < layers.size(); ++i)
    {
        int numInputNeurons = layers[i - 1];
        int numOutputNeurons = layers[i];

        // Allocate memory for the output of this layer
        cudaMalloc(&d_output, numOutputNeurons * sizeof(float));

        // Set up grid and block dimensions for kernel
        int blockSize = 128; // A common choice, you might need to tune this
        int numBlocks = (numOutputNeurons + blockSize - 1) / blockSize;

        // Launch the kernel for this layer
        MatrixVectorMultiplyAndTanhKernel<<<numBlocks, blockSize>>>(d_weights, d_input, d_output, numOutputNeurons, numInputNeurons);

        // Wait for GPU to finish before continuing
        cudaDeviceSynchronize();

        // Update d_input to point to the output of this layer
        // The output of this layer is the input to the next layer
        d_input = d_output;

        // Optionally, you can manage the memory more efficiently by reusing or freeing d_output
    }

    // Copy the final output back to host
    std::vector<float> output(layers.back());
    cudaMemcpy(output.data(), d_output, output.size() * sizeof(float), cudaMemcpyDeviceToHost);

    // Free the last d_output allocated inside the loop
    cudaFree(d_output);

    return output;
}

void NeuralNetworkCUDA::Mutate(float mutationRate, float mutationScale)
{
    size_t totalWeightsSize = 0;
    for (size_t i = 1; i < layers.size(); ++i)
    {
        totalWeightsSize += layers[i] * layers[i - 1];
    }

    int blockSize = 256;
    int numBlocks = (totalWeightsSize + blockSize - 1) / blockSize;

    // Use a fixed seed for reproducibility, or vary it for different results
    unsigned long long seed = 1234;

    MutateKernel<<<numBlocks, blockSize>>>(d_weights, totalWeightsSize, mutationRate, mutationScale, seed);

    cudaDeviceSynchronize();
}

float NeuralNetworkCUDA::GetFitness() const
{
    return fitness;
}

void NeuralNetworkCUDA::AddFitness(float fit)
{
    fitness += fit;
}

void NeuralNetworkCUDA::SetFitness(float fit)
{
    fitness = fit;
}

void NeuralNetworkCUDA::PrintWeights() const
{
    // Print weights (may require copying data back to host)
    // This is a placeholder for actual implementation
}

int NeuralNetworkCUDA::CompareTo(const NeuralNetworkCUDA &other) const
{
    return fitness > other.fitness ? 1 : (fitness < other.fitness ? -1 : 0);
}

float NeuralNetworkCUDA::RandomWeight()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-0.5f, 0.5f); // Example: normal distribution with mean 0 and stddev 1
    return dist(gen);
}
