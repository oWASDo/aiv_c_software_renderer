#include "aiv_math.h"

Vector3_t Vector3_new(float x, float y, float z)
{
    Vector3_t vector3 = {.x = x, .y = y, .z = z};
    return vector3;
}

Vector3_t Vector3_zero()
{
    Vector3_t vector3 = {0, 0, 0};
    return vector3;
}

float Lerp(float start, float end, float gradient)
{
    return start + (end - start) * gradient;
}

Vector3_t LerpVector3(Vector3_t start, Vector3_t end, float gradient)
{
    Vector3_t v;
    v.x = start.x + (end.x - start.x) * gradient;
    v.y = start.y + (end.y - start.y) * gradient;
    v.z = start.z + (end.z - start.z) * gradient;
    return v;
}

float Distance(int startX, int startY, int endX, int endY)
{
    return sqrt(pow((endX - startX), 2) + pow((endY - startY), 2));
}

Vector3_t SumOfVector(Vector3_t v0, Vector3_t v1){
    Vector3_t v2;
    v2.x = v0.x + v1.x;
    v2.y = v0.y + v1.y;
    v2.z = v0.z + v1.z;

    return v2;
}

Vector3_t SubOfVector(Vector3_t v0, Vector3_t v1){
    Vector3_t v2;
    v2.x = v0.x - v1.x;
    v2.y = v0.y - v1.y;
    v2.z = v0.z - v1.z;

    return v2;
}