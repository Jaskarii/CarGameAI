// NeuralNetwork.h - Header file

#include <vector>
#include <random>

class NeuralNetwork
{
public:
    NeuralNetwork(const std::vector<int> &layers);
    void CopyWeights(const NeuralNetwork &copyFrom);
    std::vector<float> FeedForward(const std::vector<float> &inputs);
    void Mutate();
    void AddFitness(float fit);
    void SetFitness(float fit);
    float GetFitness() const;
    int CompareTo(const NeuralNetwork &other) const;

private:
    std::vector<int> layers;
    std::vector<std::vector<float>> neurons;
    std::vector<std::vector<std::vector<float>>> weights;
    float fitness;

    void InitNeurons();
    void InitWeights();
    static float RandomWeight(); // Function to generate a random weight
};
