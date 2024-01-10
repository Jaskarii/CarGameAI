#ifndef CARHANLDER_H
#define CARHANLDER_H

#include "CarHandler.h"
#include "VertexBufferLayout.h"

CarHandler::CarHandler()
{
    cars = new std::vector<Car>();
}

int yMaxIndex = 0;

void CarHandler::Render(glm::mat4 proj, glm::mat4 mvp)
{
    for (size_t i = 0; i < cars->size(); i++)
    {
                cars->at(i).Update();

        carVertices[i] = cars->at(i).GetStatus();
    }
    carVertices[yMaxIndex].isCamera = 1;
    
    vbo->Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, cars->size() * sizeof(CarVertex), carVertices);
    va->Bind();
    shader->Bind();
    shader->SetUniformMat4f("proj", proj);
    shader->SetUniformMat4f("mvp", mvp);

    glDrawArrays(GL_POINTS, 0, cars->size());
}

void CarHandler::AddCar(Car car)
{
    cars->push_back(car);
}

void CarHandler::InitBuffers()
{
    int carCount = cars->size();
    carVertices = new CarVertex[carCount];
    vbo = new VertexBuffer(carVertices, carCount * sizeof(CarVertex));

    shader = new Shader("../shaders/Car.glsl");
    va = new Vertexarray();
    VertexBufferLayout layout;
    layout.PushFloat(2);
    layout.PushFloat(2);
    layout.PushUInt(1);

    va->AddBuffer(*vbo, layout);
}

std::vector<Car>* CarHandler::GetCars()
{
    return cars;
}

void CarHandler::ResetCars()
{
    for (size_t i = 0; i < cars->size(); i++)
    {
        cars->at(i).Reset();
    }
}

#endif