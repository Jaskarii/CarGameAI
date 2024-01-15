#include "CarGame.h"
#include <iostream>
#include <algorithm>
#include <mutex>
#include <thread>
#include <chrono>

NeuralNetwork *CarGame::globalBestNetwork = nullptr;
std::mutex lock;

CarGame::CarGame(int amountOfCars, bool isTraining, bool isControl) : training(isTraining), control(isControl)
{
    cars = new std::vector<Car>();
    networks = new std::vector<NeuralNetwork>();
    road = new Road();

    std::vector<int> layers = {5, 15, 15, 15, 2};
    bestNetwork = new NeuralNetwork(layers);
    bestNetwork->SetFitness(-1000000);
    for (int i = 0; i < amountOfCars; ++i)
    {
        Car car(0, i * 100); // Create a Car object and initialize it
        car.isTraining = training;
        cars->push_back(car);
        NeuralNetwork network(layers);
        networks->push_back(network);
    }
    Reset();
}

void CarGame::GameLoop()
{
    // Initialize variables to track the maximum A value and the corresponding Car.
    bool EndRun = true;
    // Iterate through the vector to find the Car with the highest A value.
    float maxY = 0;
    for (size_t i = 0; i < cars->size(); i++)
    {
        Car &car = cars->at(i);
        car.Update();
        road->UpdateCarStatus(&car);
        if (!control)
        {
            car.GetAndHandleOutPuts(&(networks->at(i)));
        }
        EndRun = !car.hasAdvanced;
        if (maxY < car.getInputs()->position.y)
        {
            maxY = car.getInputs()->position.y;
            maxYIndex = i;
        }
    }

    if (training)
    {
        frames++;
        if (EndRun || frames > 1500)
        {
            NextGeneration();
            Reset();
        }
    }
}

void CarGame::Reset()
{
    frames = 0;
    for (size_t i = 0; i < cars->size(); i++)
    {
        cars->at(i).Reset();
        road->InitNewPathSegment(&(cars->at(i)), 1);
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

std::vector<Car> *CarGame::GetCars()
{
    return cars;
}

glm::vec2 CarGame::GetCameraPosition()
{
    return glm::vec2(cars->at(maxYIndex).getInputs()->position);
}

void CarGame::CopyWeightsFromBest(NeuralNetwork *toNetwork)
{
    toNetwork->CopyWeights(bestNetwork);
}

void CarGame::InitBestNetwork(std::vector<int> layers)
{
    CarGame::globalBestNetwork = new NeuralNetwork(layers);
    CarGame::globalBestNetwork->SetFitness(-100000);
}

void *CarGame::UpdateGlobalNetwork(NeuralNetwork *candidate)
{
    lock.lock();
    if (candidate->GetFitness() > CarGame::globalBestNetwork->GetFitness())
    {
        CarGame::globalBestNetwork->CopyWeights(candidate);
        CarGame::globalBestNetwork->SetFitness(candidate->GetFitness());
        std::cout << "global updated" << globalBestNetwork->GetFitness() << std::endl;
    }
    lock.unlock();
    return nullptr;
}

void *CarGame::UpdateFromGlobalNetwork(NeuralNetwork *candidate)
{
    lock.lock();
    candidate->CopyWeights(globalBestNetwork);
    lock.unlock();
    return nullptr;
}

void CarGame::StartGame(std::atomic<bool> &stopFlag)
{
    while (!stopFlag.load(std::memory_order_acquire))
    {
        GameLoop();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void CarGame::InitBuffers()
{
    int carCount = cars->size();
    carVertices = new CarVertex[carCount];
    vbo = new VertexBuffer(carVertices, carCount * sizeof(CarVertex));

    shader = new Shader("../shaders/Car.glsl");
    va = new Vertexarray();
    VertexBufferLayout layout;
    layout.PushFloat(2);
    layout.PushFloat(2);
    layout.PushUInt(1);
    va->AddBuffer(*vbo, layout);
    road->InitBuffers();
}

void CarGame::Render(glm::mat4 proj, glm::mat4 mvp)
{
    road->Render(mvp);

    for (size_t i = 0; i < cars->size(); i++)
    {
        cars->at(i).Update();
        carVertices[i] = cars->at(i).GetStatus();
    }
    carVertices[maxYIndex].isCamera = 1;

    vbo->Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, cars->size() * sizeof(CarVertex), carVertices);
    va->Bind();
    shader->Bind();
    shader->SetUniformMat4f("proj", proj);
    shader->SetUniformMat4f("mvp", mvp);

    glDrawArrays(GL_POINTS, 0, cars->size());
}

void CarGame::NextGeneration()
{
    generation++;
    // Sort networks based on fitness.
    std::sort(networks->begin(), networks->end(), [](const NeuralNetwork &a, const NeuralNetwork &b)
              {
                  return a.CompareTo(b) > 0; // Use CompareTo for comparison
              });

    float fitnn = networks->at(0).GetFitness();
    std::cout << fitnn << " " << index << std::endl;
    if (fitnn > bestNetwork->GetFitness())
    {
        bestNetwork->CopyWeights(&(networks->at(0)));
        bestNetwork->SetFitness(fitnn);
        bestNetwork->index = index;
        networkUpdateEvent.UpdateBestNetwork(bestNetwork);
        UpdateGlobalNetwork(bestNetwork);
    }

    for (size_t i = 28; i < 30; i++)
    {
        UpdateFromGlobalNetwork(&(networks->at(i)));
    }

    for (size_t i = 0; i < networks->size(); i++)
    {
        networks->at(i).SetFitness(0);
        if (i < 30)
        {
            continue;
        }
        int asd = i % 30;
        networks->at(i).CopyWeights(&(networks->at(asd)));
        float MutationRate = i / 700.0f;
        float MutationScale = i / 400.0f;
        networks->at(i).Mutate(MutationRate, MutationScale);
    }
}
