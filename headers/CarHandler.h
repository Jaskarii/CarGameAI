#include "Car.h"
#include <vector>
#include "Vertexarray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class CarHandler
{
    public:
        CarHandler();
        void Render(glm::mat4 proj, glm::mat4 view);
        void AddCar(Car car);
        void InitBuffers();
        std::vector<Car>* GetCars();
        void ResetCars();

    private:
        Vertexarray *va;
        IndexBuffer *ibo;
        VertexBuffer *vbo;
        Shader *shader;
        CarVertex* carVertices;
        std::vector<Car> *cars;
};