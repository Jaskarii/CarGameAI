#include "CudaKernels.cuh"
#include <curand_kernel.h>

__global__ void MatrixVectorMultiplyKernel(float *matrix, float *vector, float *output, int numRows, int numCols)
{
    int row = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < numRows)
    {
        float sum = 0.0;
        for (int j = 0; j < numCols; ++j)
        {
            sum += matrix[row * numCols + j] * vector[j];
        }
        output[row] = sum;
    }
}

__global__ void ApplyTanhKernel(float *vector, int size)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < size)
    {
        vector[idx] = tanh(vector[idx]);
    }
}

__global__ void MatrixVectorMultiplyAndTanhKernel(float *matrix, float *vector, float *output, int numRows, int numCols)
{
    int row = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < numRows)
    {
        float sum = 0.0;
        for (int j = 0; j < numCols; ++j)
        {
            sum += matrix[row * numCols + j] * vector[j];
        }
        output[row] = tanh(sum); // Apply tanh activation function here
    }
}

__global__ void MutateKernel(float *weights, int size, float mutationRate, float mutationScale, unsigned long long seed)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < size)
    {
        curandState state;
        curand_init(seed, idx, 0, &state);

        if (curand_uniform(&state) < mutationRate)
        {
            weights[idx] += curand_normal(&state) * mutationScale;
        }
    }
}

__global__ void FullNetworkForwardPass(float *weights, float *neurons, int *layerSizes, int numLayers, float *output)
{
    int neuronId = blockIdx.x * blockDim.x + threadIdx.x;
    float sum = 0.0;
    int weightIndex = 0;

    for (int layer = 1; layer < numLayers; ++layer)
    {
        if (neuronId < layerSizes[layer])
        {
            sum = 0.0;
            for (int j = 0; j < layerSizes[layer - 1]; ++j)
            {
                sum += weights[weightIndex + j] * neurons[j];
            }

            sum = tanh(sum); // Apply tanh activation function
            output[neuronId] = sum;

            weightIndex += layerSizes[layer - 1] * layerSizes[layer];
            neuronId += layerSizes[layer - 1];
        }
        __syncthreads(); // Synchronize threads at the end of each layer
    }
}