#include <math.h>

#include "graphics.h"
#include "mathematics.h"

typedef struct {
    i32            nSectors;
    sector        *sectors;
    GraphicsState  graphicsState;
    v2             playerPosition;
    f32            playerAngle;
} GameState;


void load_level(GameState *state)
{
    int numberOfSectors;

    FILE *f = fopen("res/level.in", "r");
    fscanf(f, "%d", &state->nSectors);

    state->sectors = malloc(state->nSectors * sizeof(sector));

    for (int i = 0; i < state->nSectors; i++) {
        struct sector *currentSector = &state->sectors[i];
        
        fscanf(
            f, "%f %f %d", 
            &currentSector->zfloor,
            &currentSector->zceil,
            &currentSector->nWalls
        );
        
        currentSector->walls = malloc(currentSector->nWalls * sizeof(wall));

        for (int j = 0; j < currentSector->nWalls; j++) {
            struct wall *currentWall = &currentSector->walls[j];
            fscanf(
                f, "%d %d %d %d %d",
                &currentWall->p1.x,
                &currentWall->p1.y,
                &currentWall->p2.x,
                &currentWall->p2.y,
                &currentWall->neighbourId
            );
        }
    }

    for (int i = 0; i < state->nSectors; i++) {
        printf(
            "%.1f %.1f %d\n", 
            state->sectors[i].zfloor,
            state->sectors[i].zceil,
            state->sectors[i].nWalls
        );

        for (int j = 0; j < state->sectors[i].nWalls; j++) {
            printf(
                "%d %d %d %d %d\n",
                state->sectors[i].walls[j].p1.x,
                state->sectors[i].walls[j].p1.y,
                state->sectors[i].walls[j].p2.x,
                state->sectors[i].walls[j].p2.y,
                state->sectors[i].walls[j].neighbourId
            );
        }
        printf("--------\n");
    }

    fclose(f);
}

void unload_level(GameState *state)
{
    state->sectors = malloc(state->nSectors * sizeof(sector));

    for (int i = 0; i < state->nSectors; i++) {
        free(state->sectors[i].walls);
    }

    free(state->sectors);
}

void* GameModule_Main(void *savedState)
{
    GameState *gameState = (GameState*)savedState;

    if (savedState == NULL) {
        Graphics_Init();

        gameState = malloc(sizeof(GameState));
        gameState->graphicsState = g_graphicsState;
        gameState->playerAngle = 90.0f;
        gameState->playerPosition = (v2) {5.0f, 5.0f};

        load_level(gameState);
    } else {
        g_graphicsState = gameState->graphicsState;
    }

    f32 moveSpeed = 0.5f;
    f32 turnSpeed = 5.0f;

    while (1) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            }
            
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_DOWN:
                        gameState->playerPosition = (v2) {
                            gameState->playerPosition.x + (moveSpeed * cos(DEG2RAD(gameState->playerAngle))),
                            gameState->playerPosition.y + (moveSpeed * sin(DEG2RAD(gameState->playerAngle)))
                        };

                        break;
                    case SDLK_UP:
                        gameState->playerPosition = (v2) {
                            gameState->playerPosition.x - (moveSpeed * cos(DEG2RAD(gameState->playerAngle))),
                            gameState->playerPosition.y - (moveSpeed * sin(DEG2RAD(gameState->playerAngle)))
                        };

                        break;
                    case SDLK_LEFT:
                        gameState->playerAngle -= turnSpeed;
                        break;
                    case SDLK_RIGHT:
                        gameState->playerAngle += turnSpeed;
                        break;
                    case SDLK_a:
                        gameState->playerAngle -= moveSpeed;
                        break;
                    case SDLK_d:
                        gameState->playerAngle += moveSpeed;
                        break;
                    case SDLK_r:
                        return gameState; 
                    case SDLK_l:
                        unload_level(gameState);
                        load_level(gameState);
                        break;
                    case SDLK_q:
                        SDL_Quit();
                        return NULL; 
                    default:
                        break;
                }
            }
        }

        Graphics_Draw(
            gameState->sectors, 
            gameState->nSectors, 0, 
            gameState->playerPosition,
            gameState->playerAngle
        );

        Graphics_Present();
    }

    return NULL;
}