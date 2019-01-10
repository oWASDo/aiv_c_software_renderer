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

float Distance(int startX, int startY, int endX, int endY)
{
    return sqrt(pow((endX - startX), 2) + pow((endY - startY), 2));
}