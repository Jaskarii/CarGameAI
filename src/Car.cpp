#include "Car.h"
#include <cmath>
#include <iostream>
#include "Road.h"
#include "../Libraries/include/glm/gtx/fast_square_root.hpp"
#include "../Libraries/include/glm/gtx/rotate_vector.hpp"

glm::vec2 dirToNextPoint;
glm::vec2 nextPoint;
glm::vec2 nextDir;

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
        speed = 0.0f;
    }
    inputs.position += inputs.direction * speed;
}

void Car::Rotate(float angle)
{
    if (std::isnan(inputs.direction.x) || std::isnan(inputs.direction.y))
    {
        int a = 0;
    }

    rotateVector(inputs.direction, angle);

    if (std::isnan(inputs.direction.x) || std::isnan(inputs.direction.y))
    {
        int b = 0;
    }
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
    }
    else
    {
        counter = 0;
    }

    return counter > 5;
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
}

CarVertex Car::GetStatus()
{
    carStatus.dirX = inputs.direction.x;
    carStatus.dirY = inputs.direction.y;
    carStatus.posX = inputs.position.x;
    carStatus.posY = inputs.position.y;
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

float maxrelativeAngle, minrelativeAngle, maxDistane, minDistance, MaxnormalizeDistanceFromRoad, MinnormalizeDistanceFromRoad = 0;

void Car::GetAndHandleOutPuts(NeuralNetwork *network)
{
    std::vector<float> inputVector;
    float relativeAngle = calculateRelativeAngle();
    float distanceToNext = glm::fastDistance(inputs.nextPoint, inputs.position) / 600;
    distanceToNext = std::max(1.0f, distanceToNext);
    float normalizeDistanceFromRoad = inputs.distanceFromRoad / 100;
    inputVector.clear();
    inputVector.push_back(relativeAngle / 3);
    inputVector.push_back(distanceToNext);
    inputVector.push_back(normalizeDistanceFromRoad);
    inputVector.push_back(speed / 4);

    maxrelativeAngle = std::max(relativeAngle, maxrelativeAngle);
    minrelativeAngle = std::min(relativeAngle, minrelativeAngle);
    maxDistane = std::max(distanceToNext, maxDistane);
    minDistance = std::min(distanceToNext, minDistance);
    MaxnormalizeDistanceFromRoad = std::max(normalizeDistanceFromRoad, MaxnormalizeDistanceFromRoad);
    MinnormalizeDistanceFromRoad = std::min(normalizeDistanceFromRoad, MinnormalizeDistanceFromRoad);

    std::vector<float> outPuts = network->FeedForward(inputVector);

    if (std::isnan(outPuts[1]) || std::isnan(outPuts[0]))
    {
        int a = 0;
    }

    Accelerate(outPuts[0] / 40);
    Rotate(outPuts[1] / 40);

    if (isTraining)
    {
        network->AddFitness(inputs.position.y / 50 + CurrentPathIndex - std::abs(relativeAngle));
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

    if (std::isnan(angle))
    {
        int u = 0;
    }

    return angle;
}