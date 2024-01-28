#include "NeuralCuda.h"
#include <cuda_runtime.h>
#include <iostream>

__global__ void FeedForwardKernel(float *inputs, float *weights, float *outputs, int inputSize, int outputSize, int weightsSize)
{
    int networkId = blockIdx.x;

    const int layerSizes[] = {32, 32, 32, 2};
    const int numLayers = sizeof(layerSizes) / sizeof(layerSizes[0]);

    int inputStartIndex = networkId * inputSize;
    int weightStartIndex = networkId * weightsSize;
    int outputStartIndex = networkId * outputSize;

    extern __shared__ float neuronValues[];

    for (int i = threadIdx.x; i < inputSize; i += blockDim.x)
    {
        neuronValues[i] = inputs[inputStartIndex + i];
    }
    __syncthreads();

    // Index to track the current position in the weights array
    int currentWeightIndex = weightStartIndex;

    for (int layer = 0; layer < numLayers; ++layer)
    {
        int numNeurons = layerSizes[layer];
        int prevLayerSize = (layer == 0) ? inputSize : layerSizes[layer - 1];

        if (threadIdx.x < numNeurons)
        {
            float sum = 0.0f;

            int weightIndex = currentWeightIndex + threadIdx.x * prevLayerSize;

            // Perform the weighted sum of inputs for this neuron
            for (int i = 0; i < prevLayerSize; ++i)
            {
                sum += weights[weightIndex + i] * neuronValues[i];
            }

            // Apply activation function (e.g., tanh)
            neuronValues[threadIdx.x] = tanh(sum);
        }

        // Update the currentWeightIndex for the next layer
        currentWeightIndex += numNeurons * prevLayerSize;

        __syncthreads();
    }

    if (threadIdx.x < outputSize)
    {
        outputs[outputStartIndex + threadIdx.x] = neuronValues[threadIdx.x];
    }
}

void CheckCudaError()
{
    cudaError_t err;
    err = cudaDeviceSynchronize();
    if (err != cudaSuccess)
    {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", err);
        // Handle the error, e.g., free memory
    }
}

NeuralCuda::NeuralCuda(const std::vector<int> &layerSizes, int cars)
{
    this->amountOfCars = cars;
    this->layerSizes = layerSizes;
    this->numLayers = layerSizes.size();
    this->totalWeights = 0;
    for (size_t i = 0; i < layerSizes.size() - 1; i++)
    {
        this->totalWeights += layerSizes[i] * layerSizes[i + 1];
    }

    AllocateMemory();
}

NeuralCuda::~NeuralCuda()
{
    FreeMemory();
}

void NeuralCuda::AllocateMemory()
{
    // Allocate memory for inputs and outputs as before
    cudaMalloc(&d_inputs, sizeof(float) * amountOfCars * layerSizes[0]);      // layerSizes[0] is the size of the input layer
    cudaMalloc(&d_outputs, sizeof(float) * amountOfCars * layerSizes.back()); // layerSizes.back() is the size of the output layer

    // Calculate the total number of weights, excluding the input layer
    this->totalWeights = 0;
    for (int i = 0; i < numLayers - 1; i++)
    {
        this->totalWeights += layerSizes[i] * layerSizes[i + 1];
    }

    // Allocate memory for weights
    cudaMalloc(&d_weights, sizeof(float) * this->totalWeights * amountOfCars);

    // Allocate and copy layerSizes
    cudaMalloc(&d_layerSizes, sizeof(int) * numLayers);
    cudaMemcpy(d_layerSizes, layerSizes.data(), sizeof(int) * numLayers, cudaMemcpyHostToDevice);
}

void NeuralCuda::FreeMemory()
{
    cudaFree(d_inputs);
    cudaFree(d_weights);
    cudaFree(d_outputs);
    cudaFree(d_layerSizes);
}

void NeuralCuda::SetInputs(const std::vector<float> &inputs)
{
    cudaMemcpy(d_inputs, inputs.data(), sizeof(float) * inputs.size(), cudaMemcpyHostToDevice);
}

void NeuralCuda::SetWeights(const std::vector<float> &weights)
{
    cudaMemcpy(d_weights, weights.data(), sizeof(float) * weights.size(), cudaMemcpyHostToDevice);
    CheckCudaError();
}

void NeuralCuda::FeedForward(std::vector<float> &outputVec)
{
    // dim3 blockSize(1, 2); // 1 thread along x-axis (for 1 car), 2 threads along y-axis (for 2 outputs)
    // dim3 gridSize(1, 1);  // Only 1 block along both x and y axes for 1 car

    // Assuming the largest layer has 20 neurons
    int threadsPerBlock = 32;     // or a multiple of 32 for warp alignment, like 32
    int numBlocks = amountOfCars; // one block per network

    // Calculate shared memory size if used
    int sharedMemorySize = sizeof(float) * 32; // maxLayerSize is the size of the largest layer

    FeedForwardKernel<<<numBlocks, threadsPerBlock, sharedMemorySize>>>(d_inputs, d_weights, d_outputs, layerSizes[0], layerSizes.back(), this->totalWeights);

    // Assuming the output size is known and fixed (e.g., 300 * 2)
    std::vector<float> outputs(amountOfCars * 2);
    cudaMemcpy(outputVec.data(), d_outputs, sizeof(float) * outputVec.size(), cudaMemcpyDeviceToHost);

    CheckCudaError();
}

// Main function or other functions to interact with the NeuralNetwork class
