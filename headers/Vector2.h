#ifndef VECTOR_H
#define VECTOR_H

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp> // For glm::length2

static float DistanceToLineSegment(const glm::vec2 &P, const glm::vec2 &A, const glm::vec2 &B)
{
    glm::vec2 AP = P - A, AB = B - A;
    float AB_squared = glm::length2(AB); // Squared length of AB

    // Handle the case where A and B are the same point
    if (AB_squared == 0.0f)
    {
        return glm::length(AP);
    }

    // Calculate the dot product and the projection factor
    float projection = glm::dot(AP, AB) / AB_squared;

    // Check if P projects outside the line segment
    if (projection < 0.0f)
    {
        return glm::length(AP); // Distance to A
    }
    else if (projection > 1.0f)
    {
        return glm::length(P - B); // Distance to B
    }
    else
    {
        glm::vec2 projectionPoint = A + projection * AB; // Projection point on the line
        return glm::length(P - projectionPoint);         // Perpendicular distance to the line
    }
}
#endif