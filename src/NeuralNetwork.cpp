#include "NeuralNetwork.h"
#include <cmath>
#include <random>

NeuralNetwork::NeuralNetwork(const std::vector<int> &layers) : layers(layers), fitness(0)
{
    InitNeurons();
    InitWeights();
    Mutate(200);
}

void NeuralNetwork::CopyWeights(const NeuralNetwork &copyFrom)
{
    if (weights.size() != copyFrom.weights.size()) 
    {
        return;
    }

    for (int i = 0; i < weights.size(); ++i) 
    {
        if (weights[i].size() != copyFrom.weights[i].size() ||
            weights[i][0].size() != copyFrom.weights[i][0].size())
        {
            return;
        }

        for (int j = 0; j < weights[i].size(); ++j) 
        {
            for (int k = 0; k < weights[i][j].size(); ++k) 
            {
                weights[i][j][k] = copyFrom.weights[i][j][k];
            }
        }
    }
}

void NeuralNetwork::InitNeurons()
{
    neurons.clear();
    for (auto layer_size : layers)
    {
        neurons.push_back(std::vector<float>(layer_size, 0.0f));
    }
}

void NeuralNetwork::InitWeights()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-0.5f, 0.5f);

    weights.clear();
    for (size_t i = 1; i < layers.size(); ++i)
    {
        std::vector<std::vector<float>> layer_weights;
        int neurons_in_previous_layer = layers[i - 1];

        for (int j = 0; j < layers[i]; ++j)
        {
            std::vector<float> neuron_weights;
            for (int k = 0; k < neurons_in_previous_layer; ++k)
            {
                neuron_weights.push_back(dist(gen));
            }
            layer_weights.push_back(neuron_weights);
        }
        weights.push_back(layer_weights);
    }
}

std::vector<float> NeuralNetwork::FeedForward(const std::vector<float> &inputs)
{
    for (size_t i = 0; i < inputs.size(); ++i)
    {
        neurons[0][i] = inputs[i];
    }

    for (size_t i = 1; i < layers.size(); ++i)
    {
        for (size_t j = 0; j < neurons[i].size(); ++j)
        {
            float value = 0.0f;
            for (size_t k = 0; k < neurons[i - 1].size(); ++k)
            {
                value += weights[i - 1][j][k] * neurons[i - 1][k];
            }
            neurons[i][j] = std::tanh(value);
        }
    }
    return neurons.back();
}

void NeuralNetwork::Mutate(float rate)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 100);

    for (auto &layer : weights)
    {
        for (auto &neuron : layer)
        {
            for (auto &weight : neuron)
            {
                float randomNumber = dist(gen);
                if (randomNumber > 98)
                {
                    weight = RandomWeight();
                }
                else if (randomNumber > 90)
                {
                    weight = -weight;
                }
                else if (randomNumber > 70)
                {
                    weight += 0.1;
                }
                else if (randomNumber > 50)
                {
                    weight -= 0.1;
                }
                else
                {
                    continue;
                }
            }
        }
    }
}

float NeuralNetwork::RandomWeight()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(-0.5f, 0.5f);
    return dist(gen);
}

void NeuralNetwork::AddFitness(float fit)
{
    fitness += fit;
}

void NeuralNetwork::SetFitness(float fit)
{
    fitness = fit;
}

std::vector<std::vector<std::vector<float>>> *NeuralNetwork::GetWeights()
{
    return &weights;
}

float NeuralNetwork::GetFitness() const
{
    return fitness;
}

int NeuralNetwork::CompareTo(const NeuralNetwork &other) const
{
    if (fitness > other.fitness)
        return 1;
    else if (fitness < other.fitness)
        return -1;
    else
        return 0;
}