#ifndef NNE_H
#define NNE_H

#include <Eigen/Dense>
#include <vector>

class NeuralNetworkEigen
{
public:
    NeuralNetworkEigen(const std::vector<int> &layers);
    void CopyWeights(const NeuralNetworkEigen *copyFrom);
    Eigen::VectorXf FeedForward(const Eigen::VectorXf &inputs);
    void Mutate(float mutationRate, float mutationScale);
    void AddFitness(float fit);
    void SetFitness(float fit);
    float GetFitness() const;
    void printWeights() const;
    int CompareTo(const NeuralNetworkEigen &other) const;
    int index = 0;

    // Getters for weights for external access
    const std::vector<Eigen::MatrixXf> &GetWeights() const;

private:
    std::vector<int> layers;
    std::vector<Eigen::VectorXf> neurons; // Using Eigen's VectorXf for neurons
    std::vector<Eigen::MatrixXf> weights; // Using Eigen's MatrixXf for weights
    float fitness;

    void InitNeurons();
    void InitWeights();
    static float RandomWeight(); // Function to generate a random weight
};

#endif
