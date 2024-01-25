#ifndef NEURAL_NETWORK_KERNELS_CUH
#define NEURAL_NETWORK_KERNELS_CUH

__global__ void MatrixVectorMultiplyKernel(float *matrix, float *vector, float *output, int numRows, int numCols);
__global__ void ApplyTanhKernel(float *vector, int size);
__global__ void MutateKernel(float *weights, int size, float mutationRate, float mutationScale, unsigned long long seed);
__global__ void MatrixVectorMultiplyAndTanhKernel(float *matrix, float *vector, float *output, int numRows, int numCols);
__global__ void FullNetworkForwardPass(float *weights, float *neurons, int *layerSizes, int numLayers, float *output);

#endif // NEURAL_NETWORK_KERNELS_CUH
