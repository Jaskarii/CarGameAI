#ifndef CARGAME_H
#define CARGAME_H

#include "Road.h"
#include <chrono>
#include "vector"
#include "atomic"
#include "NeuralNetwork.h"
#ifdef USE_CUDA
#include "NeuralCuda.h"
#endif
class CarGame
{
public:
    CarGame(int amountOfCarsm, bool isTraining, bool isControl);
    void GameLoop();
    void Reset();
    NeuralNetworkEigen *GetBestNetwork();
    std::vector<NeuralNetworkEigen> *GetNetworks();
    std::vector<Car> *GetCars();
    glm::vec2 GetCameraPosition();
    void CopyWeightsFromBest(NeuralNetworkEigen *toNetwork);
    static void InitBestNetwork(std::vector<int> layers);
    static void *UpdateGlobalNetwork(NeuralNetworkEigen *candidate);
    static void *UpdateFromGlobalNetwork(NeuralNetworkEigen *candidate);
    void StartGame(std::atomic<bool> &stopFlag);
    int index = 0;
    static NeuralNetworkEigen *globalBestNetwork;
    void InitBuffers();
    void Render(glm::mat4 proj, glm::mat4 mvp);
    void InitializeCudaNetwork();

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
#ifdef USE_CUDA
    NeuralCuda *cudaNN;
#endif
    std::vector<float> *allInputs;
    std::vector<float> *allOutputs;
    bool training;
    Vertexarray *va;
    IndexBuffer *ibo;
    VertexBuffer *vbo;
    Shader *shader;
    CarVertex *carVertices;
    std::vector<NeuralNetworkEigen> *networks;
    bool control;
    Road *road;
    int maxYIndex = 0;
    std::vector<int> layer;
    std::vector<Car> *cars;
    NeuralNetworkEigen *bestNetwork;
    int generation = 0;
    void NextGeneration();
    int frames = 0;
    int loops = 0;
};
#endif