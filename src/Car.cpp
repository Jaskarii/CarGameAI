#include "Car.h"
#include <cmath>
#include <iostream>
#include "VertexBufferLayout.h"

Car::Car()
{
    float temp_positions[] = {
        -0.1f, 0.7f};

        positions = new float[2]; // Allocate memory for 2 floats (adjust size accordingly)
    std::copy(std::begin(temp_positions), std::end(temp_positions), positions);

    vbo = new VertexBuffer(positions, 2 * sizeof(float));

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

void Car::Render(GLFWwindow *window)
{

    if (speed > 0)
    {
        vbo->Bind();
        positions[0] += dirX * speed;
        positions[1] += dirY * speed;
        glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(float), positions);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        rotateVector(&dirX, &dirY, 0.05f);
    }
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        rotateVector(&dirX, &dirY, -0.05f);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        speed += 0.0001f;
        speed = std::min(speed, 0.01f);
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        speed -= 0.0001f;
        speed = std::max(speed, 0.0f);
    }

    va->Bind();
    shader->Bind();
    shader->SetUniform2f("direction", dirX, dirY);

    glDrawArrays(GL_POINTS, 0, 1);
}

Vector2 Car::GetPosition()
{
    return Vector2(positions[0], positions[1]);
}
