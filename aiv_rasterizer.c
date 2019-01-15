#include "aiv_rasterizer.h"
#include <string.h>
#include <stdio.h>

#include <math.h>



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

void PutPixel(Context_t *ctx, int x, int y, Vector3_t color)
{
    if (x < 0 || y < 0 || x >= ctx->width || y >= ctx->height)
        return;

    int offset = ((y * ctx->width) + x) * 4;
    ctx->framebuffer[offset + 3] = (char)color.x;
    ctx->framebuffer[offset + 1] = (char)color.y;
    ctx->framebuffer[offset + 2] = (char)color.z;
    ctx->framebuffer[offset] = 255;
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

static void DrawTriangle_Slope(Context_t ctx, Triangle_t triangle, void (**ptr)(Context_t, int, int, int, Vector3_t, Vector3_t))
{
    float slope0 = Slope(triangle.a.raster_x, triangle.a.raster_y, triangle.b.raster_x, triangle.b.raster_y);
    float slope1 = Slope(triangle.a.raster_x, triangle.a.raster_y, triangle.c.raster_x, triangle.c.raster_y);
    int slope = slope0 >= slope1 ? 1 : 0;

    int i = 0;
    int dist = triangle.b.raster_y - triangle.a.raster_y;
    int dist2 = triangle.c.raster_y - triangle.a.raster_y;
    int dist3 = 0;

    for (i = triangle.a.raster_y; i < triangle.a.raster_y + dist; i++)
    {
        float gradient = 1;

        if(triangle.b.raster_y != triangle.a.raster_y){
            gradient = (i - triangle.a.raster_y) / (float)dist;
        }

        float X0 = Lerp(triangle.a.raster_x, triangle.b.raster_x, gradient);

        Vector3_t c0 = LerpVector3(triangle.a.color, triangle.b.color, gradient);
        PutPixel(&ctx, X0, i, c0);

        gradient = 1;
        if(triangle.c.raster_y != triangle.a.raster_y){
            gradient = (i - triangle.a.raster_y) / (float)dist2;
        }

        float X1 = Lerp(triangle.a.raster_x, triangle.c.raster_x, gradient);
        
        Vector3_t c1 = LerpVector3(triangle.a.color, triangle.c.color, gradient);
        PutPixel(&ctx, X1, i, c1);

        ptr[slope](ctx, i, X0, X1, c1, c0);
    }

    dist = triangle.c.raster_y - triangle.b.raster_y;
    dist2 = triangle.c.raster_y - triangle.a.raster_y;
    dist3 = 0;

    for (i = triangle.b.raster_y; i < triangle.b.raster_y + dist; i++)
    {
        float gradient = (i - triangle.b.raster_y) / (float)dist;

        float X0 = Lerp(triangle.b.raster_x, triangle.c.raster_x, gradient);

        Vector3_t c0 = LerpVector3(triangle.b.color, triangle.c.color, gradient);
        PutPixel(&ctx, X0, i, c0);

        gradient = (i - triangle.a.raster_y) / (float)dist2;
        float X1 = Lerp(triangle.a.raster_x, triangle.c.raster_x, gradient);

        Vector3_t c1 = LerpVector3(triangle.a.color, triangle.c.color, gradient);
        PutPixel(&ctx, X1, i, c1);

        ptr[slope](ctx, i, X0, X1, c1, c0);
    }
}

static void FullTriangleDX(Context_t ctx, int i, int X0, int X1, Vector3_t color, Vector3_t color2)
{
    int dist3 = X0 - X1;
    int j = 0;
    for (j = X1; j < X1 + dist3; j++)
    {
        float gradient = (j - X1) / (float)dist3;
        Vector3_t color3 = LerpVector3(color, color2, gradient);

        PutPixel(&ctx, j, i, color3);
    }
}

static void FullTriangleSX(Context_t ctx, int i, int X0, int X1, Vector3_t color, Vector3_t color2)
{
    int dist3 = X1 - X0;
    int j = 0;
    for (j = X0; j < X0 + dist3; j++)
    {
        float gradient = (j - X0) / (float)dist3;
        Vector3_t color3 = LerpVector3(color2, color, gradient);

        PutPixel(&ctx, j, i, color3);
    }
}

static void ViewToRasterForVertex(Context_t ctx, Vertex_t* vertex, Camera_c camera, float cameraDistance){
    
    float projectionX = vertex->viewPosition.x / (cameraDistance * vertex->viewPosition.z);
    float projectionY = vertex->viewPosition.y  / (cameraDistance * vertex->viewPosition.z);

    vertex->raster_x = (projectionX + 1) * (ctx.width * 0.5);
    vertex->raster_y = ctx.height - ((projectionY +1 ) * (ctx.height * 0.5));

}

static void ViewToRaster(Context_t ctx, Triangle_t* triangle, Camera_c camera){

    float fov = (camera.fow * 0.5) * (3.1415926535 / 180.0);
    float camraDistance = tan(fov);

    ViewToRasterForVertex(ctx, &triangle->a, camera, camraDistance);
    ViewToRasterForVertex(ctx, &triangle->b, camera, camraDistance);
    ViewToRasterForVertex(ctx, &triangle->c, camera, camraDistance);

   
}
void DrawTriangle(Context_t ctx, Triangle_t triangle, Camera_c camera)
{
    triangle.a.viewPosition = SubOfVector(triangle.a.position, camera.position);
    triangle.c.viewPosition = SubOfVector(triangle.c.position, camera.position);
    triangle.b.viewPosition = SubOfVector(triangle.b.position, camera.position);
   
   printf("X:%f ", triangle.b.viewPosition.x);
   printf("Y:%f ", triangle.b.viewPosition.y);
   printf("Z:%f \n", triangle.b.viewPosition.z);
   printf("________\n");
   printf("________\n");
   printf("________\n");
   printf("________\n");

   printf("X:%f ", triangle.b.position.x);
   printf("Y:%f ", triangle.b.position.y);
   printf("Z:%f \n", triangle.b.position.z);
    //triangle.a.viewPosition = SubOfVector(camera.position, triangle.a.position);
    //triangle.b.viewPosition = SubOfVector(camera.position, triangle.b.position);
    //triangle.c.viewPosition = SubOfVector(camera.position, triangle.c.position );



    ViewToRaster(ctx, &triangle, camera);
    //rasterize(&ctx, &triangle);
    YOrderTriangle(&triangle);

    void (*ptr[2])(Context_t, int, int, int, Vector3_t, Vector3_t);
    ptr[0] = &FullTriangleSX;
    ptr[1] = &FullTriangleDX;
    DrawTriangle_Slope(ctx, triangle, ptr);
}
