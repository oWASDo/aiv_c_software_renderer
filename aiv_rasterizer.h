#include "aiv_math.h"




typedef struct Context
{
    int width;
    int height;

    unsigned char *framebuffer;

} Context_t;

typedef struct Vertex
{
    Vector3_t position;
    Vector3_t normal;
    Vector3_t color;

    Vector3_t viewPosition;

    int raster_x;
    int raster_y;
} Vertex_t;

Vertex_t Vertex_new(Vector3_t position);

typedef struct Triangle
{
    Vertex_t a;
    Vertex_t b;
    Vertex_t c;
} Triangle_t;


typedef struct Camera
{
    Vector3_t position;
    float fow;

}Camera_c;

Triangle_t Triangle_new(Vertex_t a, Vertex_t b, Vertex_t c);

void rasterize(Context_t *ctx, Triangle_t *triangle);

void PutPixel(Context_t *ctx, int x, int y, Vector3_t color);

void PixelConverter(Vertex_t *vertex,  Context_t *ctx);

void ClearBuffer(Context_t *ctx, size_t size);

float Gradient(int A, int B, int C);

void YOrderTriangle(Triangle_t *triangle);

float Slope(float X0, float Y0, float X1, float Y1);

void DrawTriangle(Context_t ctx, Triangle_t triangle, Camera_c c);