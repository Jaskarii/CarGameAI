#include "NeuralNetworkEigen.h"
#include <random>
#include <iostream>
#include <fstream>

NeuralNetworkEigen::NeuralNetworkEigen(const std::vector<int> &layers)
    : layers(layers), fitness(0)
{
    InitNeurons();
    InitWeights();
}

void NeuralNetworkEigen::CopyWeights(const NeuralNetworkEigen *copyFrom)
{
    if (!copyFrom)
        return; // Check for null pointer

    // Ensuring that the number of layers and their sizes match
    if (this->layers.size() != copyFrom->layers.size())
    {
        throw std::runtime_error("Layer size mismatch between neural networks.");
    }

    for (size_t i = 0; i < this->layers.size(); ++i)
    {
        if (this->layers[i] != copyFrom->layers[i])
        {
            throw std::runtime_error("Layer configuration mismatch between neural networks.");
        }
    }

    // Copying weights
    this->weights.clear(); // Clear current weights
    for (const auto &weightMatrix : copyFrom->weights)
    {
        this->weights.push_back(weightMatrix); // Copy each weight matrix
    }
}

void NeuralNetworkEigen::InitNeurons()
{
    neurons.clear();
    for (int layer_size : layers)
    {
        neurons.push_back(Eigen::VectorXf::Zero(layer_size));
    }
}

void NeuralNetworkEigen::InitWeights()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-0.5f, 0.5f);

    weights.clear();
    for (size_t i = 1; i < layers.size(); ++i)
    {
        weights.push_back(Eigen::MatrixXf::Random(layers[i], layers[i - 1]));
    }
}

Eigen::VectorXf NeuralNetworkEigen::FeedForward(const Eigen::VectorXf &inputs)
{
    neurons[0] = inputs;

    for (size_t i = 1; i < layers.size(); ++i)
    {
        // Perform the matrix-vector multiplication
        Eigen::VectorXf weightedInput = weights[i - 1] * neurons[i - 1];

        // Apply the activation function (tanh) to each element using a loop
        for (int j = 0; j < weightedInput.size(); ++j)
        {
            neurons[i](j) = std::tanh(weightedInput(j));
        }
    }

    // Return the output of the last layer
    return neurons.back();
}

void NeuralNetworkEigen::Mutate(float mutationRate, float mutationScale)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    std::normal_distribution<float> mutationDist(0.0f, mutationScale);

    for (Eigen::MatrixXf &weightMatrix : weights)
    {
        for (int i = 0; i < weightMatrix.rows(); ++i)
        {
            for (int j = 0; j < weightMatrix.cols(); ++j)
            {
                if (dist(gen) < mutationRate)
                {
                    weightMatrix(i, j) += mutationDist(gen);
                }
            }
        }
    }
}

void NeuralNetworkEigen::AddFitness(float fit)
{
    fitness += fit;
}

void NeuralNetworkEigen::SetFitness(float fit)
{
    fitness = fit;
}

float NeuralNetworkEigen::GetFitness() const
{
    return fitness;
}

const std::vector<Eigen::MatrixXf> &NeuralNetworkEigen::GetWeights() const
{
    return weights;
}

void NeuralNetworkEigen::printWeights() const
{
    std::ofstream outputFile("weights.txt");
    if (!outputFile.is_open())
    {
        std::cout << "Failed to open the output file." << std::endl;
        return;
    }

    for (const auto &layer : weights)
    {
        for (int i = 0; i < layer.rows(); ++i)
        {
            for (int j = 0; j < layer.cols(); ++j)
            {
                outputFile << layer(i, j) << " ";
            }
            outputFile << "\n";
        }
        outputFile << "\n";
    }
}

int NeuralNetworkEigen::CompareTo(const NeuralNetworkEigen &other) const
{
    if (fitness > other.fitness)
        return 1;
    else if (fitness < other.fitness)
        return -1;
    else
        return 0;
}

float NeuralNetworkEigen::RandomWeight()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    return dist(gen);
}
