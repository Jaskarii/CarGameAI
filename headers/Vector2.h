#include <cmath>

struct Vector2 
{
    float x;
    float y;
    
    Vector2(float _x, float _y) : x(_x), y(_y) {}
    
    // You can also add other member functions like vector operations
    // For example, adding two Vector2 objects:
    Vector2 operator+(const Vector2& other) const 
    {
        return Vector2(x + other.x, y + other.y);
    }

    static float DistanceToPointFromLine(Vector2 a, Vector2 b, Vector2 point)
    {
        float numerator = fabs((b.y - a.y) * point.x - (b.x - a.x) * point.y + b.x * a.y - b.y * a.x);
        float denominator = sqrt((b.y - a.y) * (b.y - a.y) + (b.x - a.x) * (b.x - a.x));
        float distance = numerator / denominator;
        return distance;
    }
};