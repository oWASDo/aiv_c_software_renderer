#include "aiv_renderer.h"
#include <stdlib.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>

#define triangle(x0, y0, z0, x1, y1, z1, x2, y2, z2) \
    Triangle_new(                                    \
        Vertex_new(Vector3_new(x0, y0, z0)),         \
        Vertex_new(Vector3_new(x1, y1, z1)),         \
        Vertex_new(Vector3_new(x2, y2, z2)))

static void Rotate(Vertex_t *vertex, Vector3_t pivot, float value)
{
    float X = vertex->position.x - pivot.x;
    float Y = vertex->position.y - pivot.y;

    float length = sqrt(X * X + Y * Y);
    float normX = (float)X / length;
    float normY = (float)Y / length;

    float ang = atan2(normY, normX);
    ang += value;

    float newSin = sin(ang);
    float newCos = cos(ang);

    Y = newSin * length;
    X = newCos * length;

    vertex->position.x = X;
    vertex->position.y = Y;
}

int main(int argc, char **argv)
{
    Context_t ctx;
    ctx.width = 600;
    ctx.height = 600;

    ctx.framebuffer = NULL;

    Triangle_t triangle = triangle(0, 0.5, 0, 0.5, 0, 0, 0, -0.5, 0);
    triangle.a.color = Vector3_new(255, 0, 0);
    triangle.b.color = Vector3_new(0, 255, 0);
    triangle.c.color = Vector3_new(0, 0, 255);

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 600, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 600, 600);

    Vector3_t pivot = Vector3_zero();

    for (;;)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                return 0;
        }

        int pitch;
        SDL_LockTexture(texture, NULL, (void **)&ctx.framebuffer, &pitch);

        ClearBuffer(&ctx, pitch * ctx.height);

        //Rotate(&triangle.a, pivot, 0.001);
        //Rotate(&triangle.b, pivot, 0.001);
        //Rotate(&triangle.c, pivot, 0.001);


        DrawTriangle(ctx, triangle);

        SDL_UnlockTexture(texture);

        SDL_RenderCopy(renderer, texture, NULL, NULL);

        SDL_RenderPresent(renderer);
    }

    return 0;
}