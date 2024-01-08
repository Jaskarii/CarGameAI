#include "Car.h"
#include <cmath>
#include <iostream>
#include "VertexBufferLayout.h"



Car::Car(float posX, float posY)
{
    carStatus.posX = posX;
    carStatus.posY = posY;
    std::vector<int> layers = {6, 15, 15, 15, 4};
    network = new NeuralNetwork(layers);
}

Car::~Car()
{
}

void rotateVector(float *X, float *Y, float angle)
{
    float rotatedX = *X * cos(angle) - *Y * sin(angle);
    float rotatedY = *X * sin(angle) + *Y * cos(angle);
    *X = rotatedX;
    *Y = rotatedY;
}

void Car::Update()
{
    if (isCrashed)
    {
        speed = std::min(speed, 0.1f);
    }

    carStatus.posX += carStatus.dirX * speed;
    carStatus.posY += carStatus.dirY * speed;
}

glm::vec2 Car::GetPosition()
{
    return glm::vec2(carStatus.posX, carStatus.posY);
}

void Car::Rotate(float angle)
{
    rotateVector(&carStatus.dirX, &carStatus.dirY, angle);
}

void Car::Accelerate(float acc)
{
    speed += acc;
    speed = std::min(speed, 4.0f);
    speed = std::max(speed, 0.0f);
}

void Car::SetCrashed(bool crashed)
{
    isCrashed = crashed;
}

bool Car::IsCrashed()
{
    return isCrashed;
}

CarVertex Car::GetStatus()
{
    return carStatus;
}

void Car::SetCamera(bool isCam)
{
    carStatus.isCamera = 1;
}
