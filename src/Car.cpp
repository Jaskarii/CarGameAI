#include "Car.h"
#include <cmath>
#include <iostream>
#include "Road.h"
#include "../Libraries/include/glm/gtx/fast_square_root.hpp"
#include "../Libraries/include/glm/gtx/rotate_vector.hpp"

Car::Car(float posX, float posY)
{
    Reset();
}

Car::~Car()
{
}

void rotateVector(glm::vec2 &v, float angle)
{
    // Convert vec2 to vec3 for rotation
    glm::vec3 v3 = glm::vec3(v, 0.0f);

    // Rotate around the Z-axis
    v3 = glm::rotateZ(v3, angle);

    // Update the original vec2
    v.x = v3.x;
    v.y = v3.y;
}

void Car::Update()
{
    if (isCrashed)
    {
        speed = 0.02f;
    }

    inputs.position += glm::fastNormalize(inputs.direction) * speed;
}

void Car::Rotate(float angle)
{
    angle = std::max(angle, -0.3f);
    angle = std::min(angle, 0.3f);

    rotateVector(inputs.direction, angle);
}

void Car::Accelerate(float acc)
{
    speed += acc;
    speed = std::min(speed, 2.0f);
    speed = std::max(speed, 0.0f);
}

void Car::SetCrashed(bool crashed)
{
    isCrashed = crashed;
}

int maxxy = 0;
int counter = 0;

bool Car::IsCrashed()
{
    return isCrashed;
}

void Car::Reset()
{
    inputs.position.x = 0.0f;
    inputs.position.y = 0.0f;
    inputs.direction.x = 0.0f;
    inputs.direction.y = 1.0f;
    inputs.roadDirection.x = 0.0f;
    inputs.roadDirection.y = 1.0f;
    carStatus.isCamera = 0;
    inputs.nextPoint.x = 0;
    inputs.nextPoint.y = 200;
    inputs.nextPointAfter.x = 100;
    inputs.nextPointAfter.y = 300;
    speed = 0;
    CurrentPathIndex = 1;
    isCrashed = false;
    previousPathIndex = 1;
    maxxy = 0;
    counter = 0;
    prevDistance = 0;
}

CarVertex Car::GetStatus()
{
    carStatus.dirX = inputs.direction.x;
    carStatus.dirY = inputs.direction.y;
    carStatus.posX = inputs.position.x;
    carStatus.posY = inputs.position.y;
    carStatus.speed = speed;
    return carStatus;
}

void Car::SetCamera(bool isCam)
{
    carStatus.isCamera = 1;
}

InputSpace *Car::getInputs()
{
    return &inputs;
}

float fit1Max = 0, fit1Min = 0, fit2Max = 0, fit2Min = 0, fit3Max = 0, fit3Min = 0;
float maxrelativeAngle, minrelativeAngle, maxDistane, minDistance, MaxnormalizeDistanceFromRoad, MinnormalizeDistanceFromRoad = 0;

void Car::GetAndHandleOutPuts(NeuralNetwork *network)
{
    std::vector<float> inputVector;
    float relativeAngle = calculateRelativeAngle();
    relativeAngle = relativeAngle / 3.0f;
    relativeAngle = std::max(relativeAngle, -1.0f);
    relativeAngle = std::min(relativeAngle, 1.0f);
    float advanced = prevDistance - inputs.distanceToNextPoint;

    // hasAdvanced > 0.5f;
    prevDistance = inputs.distanceToNextPoint;
    float distanceToNext = inputs.distanceToNextPoint / 600.0f;
    distanceToNext = std::min(1.0f, distanceToNext);
    float normalizeDistanceFromRoad = inputs.distanceFromRoad / 100.0f;

    normalizeDistanceFromRoad = std::max(normalizeDistanceFromRoad, -1.0f);
    normalizeDistanceFromRoad = std::min(normalizeDistanceFromRoad, 1.0f);
    inputVector.clear();
    inputVector.push_back(inputs.angleOfNextIntersection);
    inputVector.push_back(relativeAngle);
    inputVector.push_back(distanceToNext);
    inputVector.push_back(normalizeDistanceFromRoad);
    inputVector.push_back(speed /2.0f);

    std::vector<float> outPuts = network->FeedForward(inputVector);

    // if(outPuts[0] > 0.5f)
    // {
    //     Accelerate(0.03f);
    // }

    // if (outPuts[1] > 0.5f)
    // {
    //     Accelerate(-0.03f);
    // }

    // if(outPuts[2] > 0.5f)
    // {
    //     Rotate(0.03f);
    // }

    // if (outPuts[3] > 0.5f)
    // {
    //     Rotate(-0.03f);
    // }

    Accelerate(outPuts[0] / 30.0f);
    Rotate(outPuts[1] / 30.0f);

    if (isTraining)
    {
        if (previousPathIndex < CurrentPathIndex)
        {
            // network->AddFitness(300);
            previousPathIndex = CurrentPathIndex;
        }

        float fitnessScore = 0.0f;
        if (!isCrashed)
        {
            fitnessScore = std::abs(inputs.position.y) / 1000.0f;
        }

        // std::cout << fitnessScore << std::endl;
        network->AddFitness(fitnessScore);
    }
    // The higher the better.
}

float Car::calculateRelativeAngle()
{
    float dot = glm::dot(inputs.direction, inputs.roadDirection);
    dot = std::max(-1.0f, std::min(1.0f, dot));

    float angle = std::acos(dot); // Angle in radians

    // Determine the direction of the turn
    float crossProduct = inputs.direction.x * inputs.roadDirection.y - inputs.direction.y * inputs.roadDirection.x;
    if (crossProduct < 0)
    {
        angle = -angle; // Turn right
    }

    return angle;
}

void Car::PreRender()
{
}
