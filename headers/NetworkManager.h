#ifndef NEURAL_H
#define NEURAL_H
#include "NeuralNetwork.h"

class NetworkManager
{
    public:
        NetworkManager();
        void InitNetworks(std::vector<int> layout, int amount);
        void NextGeneration();
        NeuralNetwork* GetNetWork(int index);
        int GetGeneration();

    private:
        std::vector<NeuralNetwork>* networks;
        int generation = 0;
};
#endif