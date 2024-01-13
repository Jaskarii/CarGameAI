#ifndef VECTOR_H
#define VECTOR_H

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp> // For glm::length2

static float DistanceToLineSegment(glm::vec2 &P, const glm::vec2 &A, const glm::vec2 &B)
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

    glm::vec2 projectionPoint;
    if (projection < 0.0f)
    {
        projectionPoint = A; // Closest to A
    }
    else if (projection > 1.0f)
    {
        projectionPoint = B; // Closest to B
    }
    else
    {
        projectionPoint = A + projection * AB; // Projection point on the line
    }

    float distance = glm::length(P - projectionPoint); // Perpendicular distance to the line

    // Calculate the cross product of AB and AP
    float crossProduct = AB.x * AP.y - AB.y * AP.x;

    // Determine if the point is to the left or right of the line segment
    if (crossProduct < 0)
    {
        distance = -distance; // Point is to the right, make distance negative
    }

    return distance;
}
#endif