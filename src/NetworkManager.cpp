#include "NetworkManager.h"
#include <iostream>
#include <vector>
#include <algorithm>

NetworkManager::NetworkManager()
{
    networks = new std::vector<NeuralNetwork>();
}

void NetworkManager::InitNetworks(std::vector<int> layout, int amount)
{
    for (size_t i = 0; i < amount; i++)
    {
        NeuralNetwork network(layout);
        networks->push_back(network);
    }
}

void NetworkManager::NextGeneration()
{
    generation++;
    //Sort networks based on fitness.
    std::sort(networks->begin(), networks->end(), [](const NeuralNetwork& a, const NeuralNetwork& b) 
    {
        return a.CompareTo(b) > 0; // Use CompareTo for comparison
    });


    for (size_t i = 0; i < 1; i++)
    {
        std::cout << networks->at(i).GetFitness() << std::endl;
    }

    if (generation%20 == 0)
    {
        networks->at(0).printWeights();
    }
    

    for (size_t i = 0; i < networks->size(); i++)
    {
        networks->at(i).SetFitness(0);
        if (i < 50)
        {
            continue;
        }
        int asd = i%50;
        networks->at(i).CopyWeights(&(networks->at(asd)));
        networks->at(i).Mutate(i);
    }
}

NeuralNetwork *NetworkManager::GetNetWork(int index)
{
    return &(networks->at(index));
}

int NetworkManager::GetGeneration()
{
    return generation;
}
