#ifndef CAR_H
#define CAR_H
#include "Shader.h"
#include <string>
#include "Vector2.h"
#include "NeuralNetwork.h"

struct InputSpace
{
    float pos1X = 0.0f;
    float pos1Y = 0.0f;
    float pos2X = 0.0f;
    float pos2Y = 0.0f;
    float pos3X = 0.0f;
    float pos3Y = 0.0f;
    float pos4X = 0.0f;
    float pos4Y = 0.0f;
    float pos5X = 0.0f;
    float pos5Y = 0.0f;
    float dirX = 0.0f;
    float dirY = 0.0f;
    float speed = 0.0f;
    float distanceFromRoad = 0.0f;
};

struct OutPuts
{
    float up = 0.0f;
    float down = 0.0f;
    float left = 0.0f;
    float right = 0.0f;
};

struct CarVertex
{
    float posX = 0.0f;
    float posY = 0.0f;
    float dirX = 0.0f;
    float dirY = 1.0f;
    unsigned int isCamera = 0;
};

class Car
{
public:
    Car(float posX, float posY);
    ~Car();
    void Update();
    glm::vec2 GetPosition();
    void Rotate(float angle);
    void Accelerate(float acc);
    void SetCrashed(bool crashed);
    bool IsCrashed();
    void Reset();
    CarVertex GetStatus();
    void SetCamera(bool isCam);
    void SetInputPositions(glm::vec2 *positions);
    InputSpace* getInputs();
    void GetAndHandleOutPuts(NeuralNetwork* network);
    float GetFitness();
    int CurrentPathIndex = 1;
    bool isTraining = true;

private:
    CarVertex carStatus;
    float *positions;
    float speed = 0;
    bool isCrashed = false;
    InputSpace inputs;
};
#endif // CAR_H