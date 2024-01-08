#ifndef CAR_H
#define CAR_H
#include <glad/glad.h>
#include "Shader.h"
#include "IndexBuffer.h"
#include "Vertexarray.h"
#include <string>
#include "glfw3.h"
#include "Vector2.h"
#include "GfxObject.h"
#include "NeuralNetwork.h"

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
    CarVertex GetStatus();
    void SetCamera(bool isCam);
    void SetInput();

private:
    CarVertex carStatus;
    float *positions;
    float speed = 0;
    bool isCrashed = false;
    NeuralNetwork* network;
};
#endif // CAR_H