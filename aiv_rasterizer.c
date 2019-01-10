#include "aiv_rasterizer.h"
#include <string.h>
#include <stdio.h>

Vertex_t Vertex_new(Vector3_t position)
{
    Vertex_t vertex;
    memset(&vertex, 0, sizeof(Vertex_t));
    vertex.position = position;
    return vertex;
}

Triangle_t Triangle_new(Vertex_t a, Vertex_t b, Vertex_t c)
{
    Triangle_t triangle = {.a = a, .b = b, .c = c};
    return triangle;
}

void PutPixel(Context_t *ctx, int x, int y)
{
    if (x < 0 || y < 0 || x >= ctx->width || y >= ctx->height)
        return;

    int offset = ((y * ctx->width) + x) * 4;
    ctx->framebuffer[offset] = 255;
    ctx->framebuffer[offset + 1] = 0;
    ctx->framebuffer[offset + 2] = 0;
    ctx->framebuffer[offset + 3] = 255;
}

void PixelConverter(Vertex_t *vertex, Context_t *ctx)
{
    vertex->raster_x = (vertex->position.x + 1) / 2 * ctx->width;
    vertex->raster_y = ((vertex->position.y * -1) + 1) / 2 * ctx->height;
}

void rasterize(Context_t *ctx, Triangle_t *triangle)
{
    PixelConverter(&triangle->a, ctx);
    PixelConverter(&triangle->b, ctx);
    PixelConverter(&triangle->c, ctx);
}

void ClearBuffer(Context_t *ctx, size_t size)
{
    memset(ctx->framebuffer, 0, size);
}

float Gradient(int A, int B, int C)
{
    int dist = abs(A - B);
    return abs(A - C) / (float)dist;
    ;
}

void YOrderTriangle(Triangle_t *triangle)
{
    Vertex_t v = triangle->a;

    if (triangle->a.raster_y >= triangle->b.raster_y)
    {
        triangle->a = triangle->b;
        triangle->b = v;
    }

    v = triangle->b;
    if (triangle->b.raster_y >= triangle->c.raster_y)
    {
        triangle->b = triangle->c;
        triangle->c = v;
    }

    v = triangle->a;
    if (triangle->a.raster_y >= triangle->b.raster_y)
    {
        triangle->a = triangle->b;
        triangle->b = v;
    }
}

float Slope(float X0, float Y0, float X1, float Y1)
{
    return (X1 - X0) / (Y1 - Y0);
}

void DrawTriangle_Slope(Context_t ctx, Vertex_t a, Vertex_t b, Vertex_t c, struct PointerFunction *f, int slope)
{
    int i = 0;
    int dist = b.raster_y - a.raster_y;
    int dist2 = c.raster_y - a.raster_y;
    int dist3 = 0;

    for (i = a.raster_y; i < a.raster_y + dist; i++)
    {
        float gradient = (i - a.raster_y) / (float)dist;
        float X0 = Lerp(a.raster_x, b.raster_x, gradient);
        PutPixel(&ctx, X0, i);

        gradient = (i - a.raster_y) / (float)dist2;
        float X1 = Lerp(a.raster_x, c.raster_x, gradient);
        PutPixel(&ctx, X1, i);

        f->slope_func[slope](ctx, i, X0, X1);
    }

    dist = c.raster_y - b.raster_y;
    dist2 = a.raster_y - c.raster_y;
    dist3 = 0;

    for (i = b.raster_y; i < b.raster_y + dist; i++)
    {
        float gradient = (i - b.raster_y) / (float)dist;
        float X0 = Lerp(b.raster_x, c.raster_x, gradient);
        PutPixel(&ctx, X0, i);

        gradient = (i - b.raster_y) / (float)dist2;
        float X1 = Lerp(a.raster_x, c.raster_x, gradient);
        PutPixel(&ctx, X1, i);

        f->slope_func[slope](ctx, i, X0, X1);
    }
}

void FullTriangleDX(Context_t ctx, int i, int X0, int X1)
{
    int dist3 = X0 - X1;
    int j = 0;
    for (j = X1; j < X1 + dist3; j++)
        PutPixel(&ctx, j, i);
}

void FullTriangleSX(Context_t ctx, int i, int X0, int X1)
{
    int dist3 = X1 - X0;
    int j = 0;
    for (j = X0; j < X0 + dist3; j++)
        PutPixel(&ctx, j, i);
}

void DrawTriangle(Context_t ctx, Triangle_t triangle)
{
    rasterize(&ctx, &triangle);
    YOrderTriangle(&triangle);

    float slope0 = Slope(triangle.a.raster_x, triangle.a.raster_y, triangle.b.raster_x, triangle.b.raster_y);
    float slope1 = Slope(triangle.a.raster_x, triangle.a.raster_y, triangle.c.raster_x, triangle.c.raster_y);
    int slope = slope0 >= slope1 ? 1 : 0;

    struct PointerFunction functions;
    functions.slope_func[0] = &FullTriangleSX;
    functions.slope_func[1] = &FullTriangleDX;

    DrawTriangle_Slope(ctx, triangle.a, triangle.b, triangle.c, &functions, slope);
}
