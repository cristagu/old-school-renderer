#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "mathematics.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define HFOV DEG2RAD(90.0f)
#define HFOV_2 DEG2RAD(45.0f)
#define VFOV 0.5f
#define EYE_Z 2.0f
#define Z_NEAR 0.001f
#define Z_FAR 1000.0f

typedef struct wall
{
    v2i p1, p2;
    i32 neighbourId;
} wall;

typedef struct sector
{
    f32   zfloor, zceil;
    i32   nWalls;
    wall *walls;
} sector;

typedef struct player 
{
    i32 currentSector;
    i32 x, y, z;
    i32 a;
    i32 l;
} player;

typedef struct GraphicsState
{
    SDL_Window   *p_window;
    SDL_Texture  *p_texture;
    SDL_Renderer *p_renderer;
} GraphicsState;

GraphicsState g_graphicsState;

void draw_debug_lines();
void Graphics_DrawText(const char* text, v2i position, u32 color);
void Graphics_Init();
void Graphics_Present();
void Graphics_Draw(sector *sectors, int nSectors, int currentSector, v2 pos, f32 a);
void Graphics_DrawPixel(v2i screenPos, u32 color);
void Graphics_Draw2dLine(v2i p1, v2i p2, u32 color);
void Graphics_DrawVerticalLine(i32 x, i32 y1, i32 y2, u32 color);

#endif