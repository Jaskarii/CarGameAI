#include "CarGame.h"
#include <iostream>
#include <algorithm>
#include <mutex>

NeuralNetwork* CarGame::globalBestNetwork = nullptr;
std::mutex lock;

CarGame::CarGame(int amountOfCars)
{
    cars = new std::vector<Car>();
    networks = new std::vector<NeuralNetwork>();
    road = new Road();

    std::vector<int> layers = {4, 10, 12, 12, 2};
    bestNetwork = new NeuralNetwork(layers);
    bestNetwork->SetFitness(-1000000);
    for (int i = 0; i < amountOfCars; ++i)
    {
        Car car(0, 0); // Create a Car object and initialize it
        cars->push_back(car);
        NeuralNetwork network(layers);
        networks->push_back(network);
    }
}

void CarGame::GameLoop()
{
    frames++;
    // Initialize variables to track the maximum A value and the corresponding Car.
    float maxY = cars->at(0).GetStatus().posY;
    bool EndRun = true;
    // Iterate through the vector to find the Car with the highest A value.
    for (size_t i = 0; i < cars->size(); i++)
    {
        Car &car = cars->at(i);
        if (!car.IsCrashed())
        {
            EndRun = false;
        }
        bool isOffRoad = road->IsOffRoad(&car);
        car.SetCrashed(isOffRoad);
        car.GetAndHandleOutPuts(&(networks->at(i)));
        if (car.getInputs()->position.y > maxY)
        {
            maxY = car.getInputs()->position.y;
        }
        car.SetCamera(false);
        car.Update();
    }

    if ((EndRun && frames > 1500))
    {
        NextGeneration();
        Reset();
    }
}

void CarGame::Reset()
{
    frames = 0;
    for (size_t i = 0; i < cars->size(); i++)
    {
        cars->at(i).Reset();
    }
}

NeuralNetwork *CarGame::GetBestNetwork()
{
    return bestNetwork;
}

std::vector<NeuralNetwork> *CarGame::GetNetworks()
{
    return networks;
}

void CarGame::InitBestNetwork(std::vector<int> layers)
{
    CarGame::globalBestNetwork = new NeuralNetwork(layers);
    CarGame::globalBestNetwork->SetFitness(-100000);
}

void *CarGame::UpdateGlobalNetwork(NeuralNetwork* candidate)
{
    lock.lock();
    if (candidate->GetFitness() > CarGame::globalBestNetwork->GetFitness())
    {
        CarGame::globalBestNetwork->CopyWeights(candidate);
        CarGame::globalBestNetwork->SetFitness(candidate->GetFitness());
    }
    else if (candidate->GetFitness() < CarGame::globalBestNetwork->GetFitness())
    {
        candidate->CopyWeights(CarGame::globalBestNetwork);
    }
    lock.unlock();
}

void CarGame::StartGame(std::atomic<bool> &stopFlag)
{
    while (!stopFlag.load(std::memory_order_acquire))
    {
        GameLoop();
    }
}

void CarGame::NextGeneration()
{
    generation++;
    // Sort networks based on fitness.
    std::sort(networks->begin(), networks->end(), [](const NeuralNetwork &a, const NeuralNetwork &b)
              {
                  return a.CompareTo(b) > 0; // Use CompareTo for comparison
              });

    for (size_t i = 0; i < 1; i++)
    {
        float fitnn = networks->at(i).GetFitness();
        if (fitnn > bestNetwork->GetFitness())
        {
            bestNetwork->CopyWeights(&(networks->at(i)));
            bestNetwork->SetFitness(fitnn);
            bestNetwork->index = index;
            networkUpdateEvent.UpdateBestNetwork(bestNetwork);
            UpdateGlobalNetwork(bestNetwork);
        }
    }

    if (networks->at(0).GetFitness() < bestNetwork->GetFitness())
    {
        for (size_t i = 0; i < 10; i++)
        {
            networks->at(i).CopyWeights(bestNetwork);
        }
    }
    
    for (size_t i = 0; i < networks->size(); i++)
    {
        networks->at(i).SetFitness(0);
        if (i < 50)
        {
            continue;
        }
        int asd = i % 50;
        networks->at(i).CopyWeights(&(networks->at(asd)));
        networks->at(i).Mutate(i);
    }
}
