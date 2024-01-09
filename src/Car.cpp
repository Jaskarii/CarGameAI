#include "Car.h"
#include <cmath>
#include <iostream>
#include "VertexBufferLayout.h"
#include "Road.h"

glm::vec2 dirToNextPoint;
glm::vec2 nextPoint;
glm::vec2 nextDir;

Car::Car(float posX, float posY)
{
    carStatus.posX = posX;
    carStatus.posY = posY;
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
        speed = 0.0f;
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
int counter = 0;
bool Car::IsCrashed()
{
    if (speed < 0.5f)
    {
        counter++;
    }else
    {
        counter = 0;
    }
    
    return counter > 5;
}

void Car::Reset()
{
    carStatus.posX = 0.0f;
    carStatus.posY = 0.0f;
    carStatus.dirX = 0.0f;
    carStatus.dirY = 1.0f;
    carStatus.isCamera = 0;
    speed = 0;
    CurrentPathIndex = 1;
}

CarVertex Car::GetStatus()
{
    return carStatus;
}

void Car::SetCamera(bool isCam)
{
    carStatus.isCamera = 1;
}

void Car::SetInputPositions(glm::vec2 *positions)
{
    inputs.pos1X = positions[CurrentPathIndex].x - carStatus.posX;
    inputs.pos1Y = positions[CurrentPathIndex].y - carStatus.posY;
    inputs.pos2X = positions[CurrentPathIndex+1].x - carStatus.posX;
    inputs.pos2Y = positions[CurrentPathIndex+1].y - carStatus.posY;

    nextDir.x = positions[CurrentPathIndex+1].x - positions[CurrentPathIndex].x;
    nextDir.y = positions[CurrentPathIndex+1].y - positions[CurrentPathIndex].y;
    nextDir = glm::normalize(nextDir);

    dirToNextPoint.x = inputs.pos1X - carStatus.posX;
    dirToNextPoint.y = inputs.pos1Y - carStatus.posY;
    dirToNextPoint = glm::normalize(dirToNextPoint);
    nextPoint.x = inputs.pos1X;
    nextPoint.y = inputs.pos1Y;
}

void Car::GetAndHandleOutPuts(NeuralNetwork* network)
{
    glm::vec2 direction(carStatus.dirX , carStatus.dirY);
    
    float dotDir = glm::dot(direction, dirToNextPoint);
    float dist = glm::distance(nextPoint, GetPosition())/40;
    network->AddFitness(carStatus.posY/100 + dotDir*10);
    std::vector<float> inputVector;
    inputVector.clear();
    inputVector.push_back(dirToNextPoint.x);
    inputVector.push_back(dirToNextPoint.y);
    inputVector.push_back(nextDir.x);
    inputVector.push_back(nextDir.y);
    inputVector.push_back(carStatus.dirX);
    inputVector.push_back(carStatus.dirY);
    inputVector.push_back(dist);
    inputVector.push_back(speed);

    std::vector<float> outPuts = network->FeedForward(inputVector);

    if (outPuts[0] > 0.0f)
    {
        Accelerate(0.03f);
    }
    else if(outPuts[1] > 0.0f)
    {
        Accelerate(-0.03f);
    }
    if (outPuts[2] > 0.0f)
    {
        Rotate(0.03f);
    }
    else if(outPuts[3] > 0.0f)
    {
        Rotate(-0.03f);
    }
}

float Car::GetFitness()
{
    return 0.0f;
}
