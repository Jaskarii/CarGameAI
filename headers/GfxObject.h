#ifndef GFXOBJECT_H
#define GFXOBJECT_H

#include "../Libraries/include/GLFW/glfw3.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class GfxObject
{
public:
    GfxObject() {}

    virtual ~GfxObject() {}

    virtual void Render(glm::mat4 MVP) = 0;

    glm::vec2 GetPosition();

    void SetTranslation(glm::vec2 transl)
    {
        translation = transl;
    }

    void SetScale(float scal)
    {
        scale = scal;
    }

protected:
    glm::vec2 position;
    glm::vec2 translation;
    float scale = 1;
    VertexBuffer *vbo;
    Shader *shader;
};

#endif