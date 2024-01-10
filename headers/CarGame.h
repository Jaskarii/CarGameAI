#ifndef CARGAME_H
#define CARGAME_H
#include "Road.h"
#include "vector"
#include "atomic"
#include "NetworkUpdateEvent.h"

class CarGame
{
public:
    CarGame(int amountOfCars);
    void GameLoop();
    void Reset();
    NeuralNetwork *GetBestNetwork();
    std::vector<NeuralNetwork> *GetNetworks();
    NetworkUpdateEvent networkUpdateEvent; // Event handler
    std::vector<Car> *GetCars();
    bool training = true;
    void StartGame(std::atomic<bool> &stopFlag);
    int index = 0;

private:
    std::vector<NeuralNetwork> *networks;
    Road *road;
    std::vector<Car> *cars;
    NeuralNetwork *bestNetwork;
    int generation = 0;
    void NextGeneration();
    int frames = 0;
    int loops = 0;
};
#endif