#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <math.h>

#include "font_8x16.h" 
#include "queue.h"
#include "graphics.h"
#include "mathematics.h"

u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

void Graphics_DrawPixel(v2i screenPos, u32 color)
{
    if (screenPos.x > 0 && screenPos.x < SCREEN_WIDTH && screenPos.y > 0 && screenPos.y < SCREEN_HEIGHT) {
        pixels[((int) screenPos.y * SCREEN_WIDTH) + (int) screenPos.x] = color;
    }
}

/**
* https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
**/
void Graphics_Draw2dLine(v2i p1, v2i p2, u32 color)
{
    i32 dx = abs(p2.x - p1.x);
    i32 sx = p1.x < p2.x ? 1 : -1;
    i32 dy = -abs(p2.y - p1.y);
    i32 sy = p1.y < p2.y ? 1 : -1;
    i32 error = dx + dy;

    while(1) {
        Graphics_DrawPixel(p1, color);
        if (p1.x == p2.x && p1.y == p2.y) { break; }

        if (2 * error >= dy) {
            if (p1.x == p2.x) { break; }
            error += dy;
            p1.x += sx;
        }

        if (2 * error <= dx) {
            if (p1.y == p2.y) { break; }
            error += dx;
            p1.y += sy;
        }

    }
}

void Graphics_DrawVerticalLine(i32 x, i32 y1, i32 y2, u32 color)
{

    for (i32 y = y1; y < y2; y++) {
        Graphics_DrawPixel((v2i) {x, y}, color);
    }

    Graphics_DrawPixel((v2i) {x, y1}, 0x000000);
    Graphics_DrawPixel((v2i) {x, y2}, 0x000000);
}

void Graphics_DrawText(const char* text, v2i position, u32 color)
{
    int index = 0;
    int height = 16;
    int width = 8;
    
    while (text[index] != 0) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if ((font8x16[text[index]][i] >> j) & 1) {
                    v2i pixel_position = {
                        position.x + (width * index) - j + width,
                        position.y - i + height
                    };

                    Graphics_DrawPixel(pixel_position, color);
                }               
            } 
        }

        index++;
    }
}

void Graphics_Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    
    g_graphicsState.p_window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0); 
    g_graphicsState.p_renderer = SDL_CreateRenderer(g_graphicsState.p_window, -1, SDL_RENDERER_PRESENTVSYNC);
    g_graphicsState.p_texture = SDL_CreateTexture(g_graphicsState.p_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Graphics_Present()
{    
    SDL_UpdateTexture(g_graphicsState.p_texture, NULL, pixels, SCREEN_WIDTH * 4); 
    SDL_RenderCopyEx(g_graphicsState.p_renderer, g_graphicsState.p_texture, NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL);
    SDL_RenderPresent(g_graphicsState.p_renderer);

    memset(pixels, 0, sizeof(pixels));
}

void Graphics_Draw(sector *sectors, int nSectors, int currentSector, v2 pos, f32 a)
{
    typedef struct {int sectorId, x1, x2, fy1, cy1, fy2, cy2;} SectorDrawRequest;

    Queue *drawRequests = CreateQueue(sizeof(SectorDrawRequest));
    Enqueue(drawRequests, &(SectorDrawRequest) {currentSector, 0, SCREEN_WIDTH - 1, 0, SCREEN_HEIGHT - 1, 0, SCREEN_HEIGHT - 1});

    int *sectorDrawn = calloc(nSectors, sizeof(int));

    while (drawRequests->length > 0) {
        SectorDrawRequest currentDrawRequest;
        Dequeue(drawRequests, &currentDrawRequest);
        sector currentSector = sectors[currentDrawRequest.sectorId];

        printf(
            "Drawing sector #%d. x1: %d x2: %d fy1: %d, cy1: %d fy2: %d cy2: %d\n",
             currentDrawRequest.sectorId,
             currentDrawRequest.x1,
             currentDrawRequest.x2,
             currentDrawRequest.fy1,
             currentDrawRequest.cy1,
             currentDrawRequest.fy2,
             currentDrawRequest.cy2
        );

        if (sectorDrawn[currentDrawRequest.sectorId] == 1) {
            printf("Skipping sector #%d \n", currentDrawRequest.sectorId);
            continue;
        }

        for (int i = 0; i < currentSector.nWalls; i++) {
            wall currentWall = currentSector.walls[i];
            v2 p1 = {currentWall.p1.x, currentWall.p1.y};
            v2 p2 = {currentWall.p2.x, currentWall.p2.y};

            p1 = Math_TranslateRotate(p1, pos, a);
            p2 = Math_TranslateRotate(p2, pos, a);

            if (p1.y < 0 && p2.y < 0) { continue; }

            v2 l = Math_Rotate((v2){1.0f, 0.0f}, PI - HFOV_2);
            v2 r = Math_Rotate((v2){1.0f, 0.0f}, HFOV_2);

            v2 znl = {l.x * Z_NEAR, l.y * Z_NEAR};
            v2 zfl = {l.x * Z_FAR, l.y * Z_FAR};
            v2 znr = {r.x * Z_NEAR, r.y * Z_NEAR};
            v2 zfr = {r.x * Z_FAR, r.y * Z_FAR};

            v2 il = Math_IntersectSegments(p1, p2, znl, zfl);
            v2 ir = Math_IntersectSegments(p1, p2, znr, zfr);

            if (!isnan(il.x)) {
                p1 = il;
            }

            if (!isnan(ir.x)) {
                p2 = ir;
            }

            f32 a1 = Math_NormalizeAngle(atan2(p1.y, p1.x) - PI_2);
            f32 a2 = Math_NormalizeAngle(atan2(p2.y, p2.x) - PI_2);

            if (a1 <= a2) { continue; }
            if ((a1 <= -HFOV_2 && a2 <= -HFOV_2) || (a1 >= HFOV_2 && a2 >= HFOV_2)) { continue; }

            i32 screenx1 = Math_ScreenAngleToX(a1, SCREEN_WIDTH, HFOV);
            i32 screenx2 = Math_ScreenAngleToX(a2, SCREEN_WIDTH, HFOV);
            i32 cscreenx1 = clamp(screenx1, currentDrawRequest.x1, currentDrawRequest.x2);
            i32 cscreenx2 = clamp(screenx2, currentDrawRequest.x1, currentDrawRequest.x2);

            f32 c1 = (VFOV * SCREEN_HEIGHT) / p1.y;
            f32 c2 = (VFOV * SCREEN_HEIGHT) / p2.y;

            i32 floory1 = (SCREEN_HEIGHT / 2) + (int) ((currentSector.zfloor - EYE_Z) * c1);
            i32 ceily1 = (SCREEN_HEIGHT / 2) + (int) ((currentSector.zceil - EYE_Z) * c1);
            i32 floory2 = (SCREEN_HEIGHT / 2) + (int) ((currentSector.zfloor - EYE_Z) * c2);
            i32 ceily2 = (SCREEN_HEIGHT / 2) + (int) ((currentSector.zceil - EYE_Z) * c2);
            i32 nfloory1, nceily1, nfloory2, nceily2; 

            if (currentWall.neighbourId != -1) {
                f32 nzceil = sectors[currentWall.neighbourId].zceil; 
                f32 nzfloor = sectors[currentWall.neighbourId].zfloor;

                nfloory1 = (SCREEN_HEIGHT / 2) + (int) ((nzfloor - EYE_Z) * c1);
                nceily1 = (SCREEN_HEIGHT / 2) + (int) ((nzceil - EYE_Z) * c1);
                nfloory2 = (SCREEN_HEIGHT / 2) + (int) ((nzfloor - EYE_Z) * c2);
                nceily2 = (SCREEN_HEIGHT / 2) + (int) ((nzceil - EYE_Z) * c2);

                Enqueue(drawRequests, &(SectorDrawRequest) {
                    currentWall.neighbourId, 
                    screenx1, 
                    screenx2, 
                    max(nfloory1, floory1),
                    min(nceily1, ceily1), 
                    max(nfloory2, floory2), 
                    min(nceily2, ceily2)
                });
            }

            for (i32 x = cscreenx1; x < cscreenx2; x++) {
                f32 delta  = (float) (x - screenx1) / (screenx2 - screenx1);
                f32 delta2 = (float) (x - currentDrawRequest.x1) / (currentDrawRequest.x2 - currentDrawRequest.x1);

                int floory = (int) (delta * (floory2 - floory1)) + floory1;
                int ceily = (int) (delta * (ceily2 - ceily1)) + ceily1;
                int nfloory = (int) (delta * (nfloory2 - nfloory1)) + nfloory1;
                int nceily = (int) (delta * (nceily2 - nceily1)) + nceily1;

                int cfloory = (int) (delta2 * (currentDrawRequest.fy2 - currentDrawRequest.fy1)) + currentDrawRequest.fy1;
                int cceily = (int) (delta2 * (currentDrawRequest.cy2 - currentDrawRequest.cy1)) + currentDrawRequest.cy1;

                floory = clamp(floory, cfloory, cceily);
                ceily = clamp(ceily, cfloory, cceily);
                nfloory = clamp(nfloory, cfloory, cceily);
                nceily = clamp(nceily, cfloory, cceily);

                if (currentWall.neighbourId == -1) {
                    if (x == screenx1 && x != 0) {
                        Graphics_DrawVerticalLine(x, floory, ceily, 0x000000);
                    } else {
                        Graphics_DrawVerticalLine(x, floory, ceily, 0xeaea00);
                    }
                } else {
                    if (x == screenx1 && x != 0) {
                        Graphics_DrawVerticalLine(x, floory, ceily, 0x000000);
                    } else {
                        Graphics_DrawVerticalLine(x, floory, nfloory, 0xeaea00);
                        Graphics_DrawVerticalLine(x, nceily, ceily, 0xeaea00);
                    }
                }

                Graphics_DrawVerticalLine(x, cfloory, floory - 1, 0xaa2b68);
                Graphics_DrawVerticalLine(x, ceily + 1, cceily, 0x46ccaa);
            }

            // draw_2d_line((v2i) {p1.x + SCREEN_WIDTH / 2, p1.y + SCREEN_HEIGHT / 2}, (v2i) {p2.x + SCREEN_WIDTH / 2, p2.y + SCREEN_HEIGHT / 2}, 0xff0000);
        }    

        sectorDrawn[currentDrawRequest.sectorId] = 1;
    }
    
    free(sectorDrawn);
}