#include "Car.h"
#include <cmath>
#include <iostream>
#include "VertexBufferLayout.h"

Car::Car()
{
    float temp_positions[] = {
        0.0f, 0.0f};

    positions = new float[2]; // Allocate memory for 2 floats (adjust size accordingly)
    std::copy(std::begin(temp_positions), std::end(temp_positions), positions);

    vbo = new VertexBuffer(positions, 4 * sizeof(float));

    shader = new Shader("../shaders/Car.glsl");

    va = new Vertexarray();
    VertexBufferLayout layout;
    layout.PushFloat(2);
    va->AddBuffer(*vbo, layout);
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

void Car::Render(glm::mat4 MVP)
{
    if (isCrashed)
    {
        speed = std::min(speed, 0.1f);
    }

    position.x += dirX * speed;
    position.y += dirY * speed;

    va->Bind();
    shader->Bind();
    shader->SetUniform2f("direction", dirX, dirY);
    shader->SetUniformMat4f("MVP", MVP);

    glDrawArrays(GL_POINTS, 0, 2);
}

glm::vec2 Car::GetPosition()
{
    return position;
}

void Car::Rotate(float angle)
{
    rotateVector(&dirX, &dirY, angle);
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
