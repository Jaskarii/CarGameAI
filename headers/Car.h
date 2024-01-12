#ifndef CAR_H
#define CAR_H
#include "Shader.h"
#include <string>
#include "Vector2.h"
#include "NeuralNetwork.h"

struct InputSpace
{
    glm::vec2 nextPoint;
    glm::vec2 nextPointAfter;
    glm::vec2 roadDirection;
    glm::vec2 position;
    glm::vec2 direction;
    float angleOfNextIntersection;
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
    void Rotate(float angle);
    void Accelerate(float acc);
    void SetCrashed(bool crashed);
    bool IsCrashed();
    void Reset();
    CarVertex GetStatus();
    void SetCamera(bool isCam);
    InputSpace *getInputs();
    void GetAndHandleOutPuts(NeuralNetwork *network);
    int CurrentPathIndex = 1;
    bool isTraining = true;
    bool hasAdvanced = true;

private:
    float calculateRelativeAngle();
    CarVertex carStatus;
    float speed = 0;
    bool isCrashed = false;
    float prevDistance = 0;
    int previousPathIndex = 1;
    InputSpace inputs;
};
#endif // CAR_H