#include <math.h>

typedef struct Vector3
{
    float x;
    float y;
    float z;
} Vector3_t;

Vector3_t Vector3_new(float x, float y, float z);

Vector3_t Vector3_zero();

float Lerp(float start, float end, float gradient);

float Distance(int startX, int startY, int endX, int endY);