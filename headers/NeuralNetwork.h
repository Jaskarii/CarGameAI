// NeuralNetwork.h - Header file
#ifndef NN_H
#define NN_H
#include <vector>
#include <random>

class NeuralNetwork
{
public:
    NeuralNetwork(const std::vector<int> &layers);
    void CopyWeights(NeuralNetwork *copyFrom);
    std::vector<float> FeedForward(const std::vector<float> &inputs);
    void Mutate(float mutationRate, float mutationScale);
    void AddFitness(float fit);
    void SetFitness(float fit);
    std::vector<std::vector<std::vector<float>>> *GetWeights();
    float GetFitness() const;
    void printWeights();
    int CompareTo(const NeuralNetwork &other) const;
    int index = 0;

private:
    std::vector<int> layers;
    std::vector<std::vector<float>> neurons;
    std::vector<std::vector<std::vector<float>>> weights;
    float fitness;

    void InitNeurons();
    void InitWeights();
    static float RandomWeight(); // Function to generate a random weight
};
#endif