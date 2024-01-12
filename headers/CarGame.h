#ifndef CARGAME_H
#define CARGAME_H

#include "Road.h"
#include "vector"
#include "atomic"
#include "NetworkUpdateEvent.h"

class CarGame
{
public:
    CarGame(int amountOfCarsm, bool isTraining, bool isControl);
    void GameLoop();
    void Reset();
    NeuralNetwork *GetBestNetwork();
    std::vector<NeuralNetwork> *GetNetworks();
    NetworkUpdateEvent networkUpdateEvent; // Event handler
    std::vector<Car> *GetCars();
    glm::vec2 GetCameraPosition();
    void CopyWeightsFromBest(NeuralNetwork *toNetwork);
    static void InitBestNetwork(std::vector<int> layers);
    static void *UpdateGlobalNetwork(NeuralNetwork *candidate);
    static void *UpdateFromGlobalNetwork(NeuralNetwork *candidate);
    void StartGame(std::atomic<bool> &stopFlag);
    int index = 0;
    static NeuralNetwork *globalBestNetwork;
    void InitBuffers();
    void Render(glm::mat4 proj, glm::mat4 mvp);

private:
    bool training;
    Vertexarray *va;
    IndexBuffer *ibo;
    VertexBuffer *vbo;
    Shader *shader;
    CarVertex* carVertices;
    std::vector<NeuralNetwork> *networks;
    bool control;
    Road *road;
    int maxYIndex = 0;
    std::vector<int> layer;
    std::vector<Car> *cars;
    NeuralNetwork *bestNetwork;
    int generation = 0;
    void NextGeneration();
    int frames = 0;
    int loops = 0;
};
#endif