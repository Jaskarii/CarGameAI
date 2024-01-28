#include "CarGame.h"
#include <iostream>
#include <algorithm>
#include <mutex>
#include <thread>

NeuralNetworkEigen *CarGame::globalBestNetwork = nullptr;
std::mutex lock;

CarGame::CarGame(int amountOfCars, bool isTraining, bool isControl) : training(isTraining), control(isControl)
{
    cars = new std::vector<Car>();
    networks = new std::vector<NeuralNetworkEigen>();
    road = new Road();

    // std::vector<int> layers = 20, 4};
    std::vector<int> layers = {5, 20, 20, 20, 2};
#ifdef USE_CUDA
    cudaNN = new NeuralCuda(layers, amountOfCars);
#endif
    bestNetwork = new NeuralNetworkEigen(layers);
    bestNetwork->SetFitness(-1000000);
    for (int i = 0; i < amountOfCars; ++i)
    {
        Car car(0, i * 100.0f); // Create a Car object and initialize it
        car.carIndex = i;
        car.isTraining = training;
        cars->push_back(car);
        NeuralNetworkEigen network(layers);
        networks->push_back(network);
    }
    allInputs = new std::vector<float>();
    allOutputs = new std::vector<float>();
    allInputs->resize(amountOfCars * layers.front());
    allOutputs->resize(amountOfCars * layers.back());
    Reset();
}

#ifdef USE_CUDA
void CarGame::InitializeCudaNetwork()
{
    std::vector<float> combinedWeights;

    for (const auto &network : *networks)
    {
        const auto &networkWeights = network.GetWeights(); // Get weights as std::vector<Eigen::MatrixXf>

        for (const auto &weightMatrix : networkWeights)
        {
            Eigen::Map<const Eigen::VectorXf> flat(weightMatrix.data(), weightMatrix.size());
            combinedWeights.insert(combinedWeights.end(), flat.data(), flat.data() + flat.size());
        }
    }

    // Now combinedWeights contains all weights flattened
    // Set these weights in your NeuralCuda instance
    cudaNN->SetWeights(combinedWeights);
}
#endif

void CarGame::GameLoop()
{
    // Initialize variables to track the maximum A value and the corresponding Car.
    bool EndRun = false;
    // Iterate through the vector to find the Car with the highest A value.
    float maxY = 0;
    // auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < cars->size(); i++)
    {
        Car &car = cars->at(i);
        car.Update();
        road->UpdateCarStatus(&car);

        if (maxY < car.getInputs()->position.y)
        {
            maxY = car.getInputs()->position.y;
            maxYIndex = i;
        }
    }
    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double, std::milli> add_vectors_duration = end - start;

    // std::cout << "Update : " << add_vectors_duration.count() << " ms  ";

    // start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < cars->size(); i++)
    {
        Car &car = cars->at(i);
        if (!control)
        {
            car.GetAndHandleOutPuts(&(networks->at(i)), allInputs);
        }
    }

#ifdef USE_CUDA
    cudaNN->SetInputs(*allInputs);
    cudaNN->FeedForward(*allOutputs);
    size_t outputIndex = 0;
    for (int i = 0; i < cars->size(); i++)
    {
        Car &car = cars->at(i);
        size_t outputIndex = i * 2; // Assuming 2 outputs per car
        // Read the outputs directly from allOutputs
        float outPut1 = allOutputs->at(outputIndex);
        float outPut2 = allOutputs->at(outputIndex + 1);
        // Use these outputs to control the car
        car.Accelerate(outPut1 / 40);
        car.Rotate(outPut2 / 30);
    }
#endif

    // end = std::chrono::high_resolution_clock::now();
    // add_vectors_duration = end - start;
    // std::cout << "Network : " << add_vectors_duration.count() << " ms  \n";

    if (training)
    {
        frames++;
        if (EndRun || frames > 2000)
        {
            NextGeneration();
            Reset();
        }
    }
}

void CarGame::Reset()
{
    std::chrono::duration<double, std::milli> add_vectors_duration = std::chrono::high_resolution_clock::now() - start;
    std::cout << "generation finished:: " << add_vectors_duration.count() << " ms" << std::endl;
    start = std::chrono::high_resolution_clock::now();
    frames = 0;
#ifdef USE_CUDA
    InitializeCudaNetwork();
#endif
    for (int i = 0; i < cars->size(); i++)
    {
        cars->at(i).Reset();
        road->InitNewPathSegment(&(cars->at(i)), 1);
    }
}

NeuralNetworkEigen *CarGame::GetBestNetwork()
{
    return bestNetwork;
}

std::vector<NeuralNetworkEigen> *CarGame::GetNetworks()
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

void CarGame::CopyWeightsFromBest(NeuralNetworkEigen *toNetwork)
{
    toNetwork->CopyWeights(bestNetwork);
}

void CarGame::InitBestNetwork(std::vector<int> layers)
{
    CarGame::globalBestNetwork = new NeuralNetworkEigen(layers);
    CarGame::globalBestNetwork->SetFitness(-100000);
}

void *CarGame::UpdateGlobalNetwork(NeuralNetworkEigen *candidate)
{
    lock.lock();
    if (candidate->GetFitness() > CarGame::globalBestNetwork->GetFitness())
    {
        CarGame::globalBestNetwork->CopyWeights(candidate);
        CarGame::globalBestNetwork->SetFitness(candidate->GetFitness());
        std::cout << "global updated " << globalBestNetwork->GetFitness() << "    ";
    }
    lock.unlock();
    return nullptr;
}

void *CarGame::UpdateFromGlobalNetwork(NeuralNetworkEigen *candidate)
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
    layout.PushFloat(1);
    va->AddBuffer(*vbo, layout);
    road->InitBuffers();
}

void CarGame::Render(glm::mat4 proj, glm::mat4 mvp)
{
    road->Render(mvp);

    for (int i = 0; i < cars->size(); i++)
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
    std::sort(networks->begin(), networks->end(), [](const NeuralNetworkEigen &a, const NeuralNetworkEigen &b)
              {
                  return a.CompareTo(b) > 0; // Use CompareTo for comparison
              });

    float fitnn = networks->at(0).GetFitness();
    if (fitnn > bestNetwork->GetFitness())
    {
        bestNetwork->CopyWeights(&(networks->at(0)));
        bestNetwork->SetFitness(fitnn);
        UpdateGlobalNetwork(bestNetwork);
    }

    for (int i = 28; i < 30; i++)
    {
        UpdateFromGlobalNetwork(&(networks->at(i)));
    }

    for (int i = 0; i < networks->size(); i++)
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
