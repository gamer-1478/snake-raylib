#include "include/raylib.h"

#define NDEBUG
#define SC_WDTH (1000)
#define SC_HGHT (1000)
#define HUD_HGHT (200)
#define GAME_HEIGHT (SC_HGHT - HUD_HGHT)
#define BLOCK_LENGTH (40)
#define NO_BLCKS_ROW (int)(SC_WDTH / BLOCK_LENGTH)
#define NO_BLCKS_COL (int)(GAME_HEIGHT / BLOCK_LENGTH)
#define MAXIMUM_SIZE_OF_QUEUE (NO_BLCKS_ROW * NO_BLCKS_COL)

typedef enum Direction
{
    none,
    up,
    right,
    down,
    left
} Direction;

typedef struct Vec2Int
{
    int x;
    int y;
} Vec2Int;

typedef struct TailPositions
{
    Vec2Int array[MAXIMUM_SIZE_OF_QUEUE];
    int size;
} TailPositions;

int score;

bool game_grid[NO_BLCKS_ROW][NO_BLCKS_COL];

Direction current_direction;
Vec2Int plyrPos;
Vec2Int tictac_pos;

TailPositions tail;

void reset_game()
{
    // set all grid blocks to false
    for (int i = 0; i < NO_BLCKS_ROW; ++i)
    {
        for (int j = 0; j < NO_BLCKS_COL; ++j)
        {
            game_grid[i][j] = false;
        }
    }

    current_direction = none;
    score = 0;
    plyrPos = (Vec2Int){7, NO_BLCKS_COL / 2};
    tictac_pos = (Vec2Int){NO_BLCKS_ROW - 5, NO_BLCKS_COL / 2};

    tail = (TailPositions){
        .array = {(Vec2Int){6, NO_BLCKS_COL / 2},
                  (Vec2Int){5, NO_BLCKS_COL / 2}},
        .size = 2,
    };
}

int main(void)
{
    // Initialization
    InitWindow(SC_WDTH, SC_HGHT, "EXUN LOCUS GAME DEV TASK 1");
    SetTargetFPS(60);

    double tick_time = 0.2;
    double time = GetTime();
    int high_score = 0;

    reset_game();

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // get input to change direction
        switch (current_direction)
        {
        default:
            break;
        case none:
            if (IsKeyPressed(KEY_UP))
            {
                current_direction = up;
            }
            else if (IsKeyPressed(KEY_RIGHT))
            {
                current_direction = right;
            }
            else if (IsKeyPressed(KEY_DOWN))
            {
                current_direction = down;
            }
            break;
        case up:
        case down:
            if (IsKeyPressed(KEY_RIGHT))
            {
                current_direction = right;
            }
            else if (IsKeyPressed(KEY_LEFT))
            {
                current_direction = left;
            }
            break;
        case right:
        case left:
            if (IsKeyPressed(KEY_UP))
            {
                current_direction = up;
            }
            else if (IsKeyPressed(KEY_DOWN))
            {
                current_direction = down;
            }
            break;
        }

        if (current_direction != none && GetTime() >= time + tick_time)
        {
            time = GetTime();

            game_grid[tail.array[tail.size - 1].x][tail.array[tail.size - 1].y] = false;

            Vec2Int next_pos = plyrPos;
            switch (current_direction)
            {
            case up:
                if (plyrPos.y == 0)
                {
                    reset_game();
                }
                else
                {
                    next_pos.y = plyrPos.y - 1;
                }
                break;
            case right:
                if (plyrPos.x == NO_BLCKS_ROW - 1)
                {
                    reset_game();
                }
                else
                {
                    next_pos.x = plyrPos.x + 1;
                }
                break;
            case down:
                if (plyrPos.y == NO_BLCKS_COL - 1)
                {
                    reset_game();
                }
                else
                {
                    next_pos.y = plyrPos.y + 1;
                }
                break;
            case left:
                if (plyrPos.x == 0)
                {
                    reset_game();
                }
                else
                {
                    next_pos.x = plyrPos.x - 1;
                }
                break;
            }

            // check next_pos does not overlap tail
            for (int i = 0; i < tail.size; ++i)
            {
                if (next_pos.x == tail.array[i].x && next_pos.y == tail.array[i].y)
                {
                    reset_game();
                }
            }

            // eat a tictac
            if (next_pos.x == tictac_pos.x && next_pos.y == tictac_pos.y)
            {
                tail.size += 1;
                score += 1;
                if (high_score < score)
                {
                    high_score = score;
                }

                // relocate tictac
                while (1)
                {
                    tictac_pos.x = GetRandomValue(0, NO_BLCKS_ROW - 1);
                    tictac_pos.y = GetRandomValue(0, NO_BLCKS_COL - 1);
                    if ((tictac_pos.x == next_pos.x && tictac_pos.y == next_pos.y) ||
                        (tictac_pos.x == plyrPos.x && tictac_pos.y == plyrPos.y))
                    {
                        continue;
                    }
                    for (int i = 0; i < tail.size; ++i)
                    {
                        if (next_pos.x == tail.array[i].x && next_pos.y == tail.array[i].y)
                        {
                            continue;
                        }
                    }
                    break;
                }
            }

            // move all tail blocks along
            for (int i = tail.size - 1;
                 i >= 0; --i)
            {
                if (i == 0)
                {
                    tail.array[i] = plyrPos;
                }
                else
                {
                    tail.array[i] = tail.array[i - 1];
                }
            }

            if (current_direction != none)
            {
                plyrPos = next_pos;
            }
        }

        // update positions of blocks
        {
            game_grid[plyrPos.x][plyrPos.y] = true;
            game_grid[tictac_pos.x][tictac_pos.y] = true;

            for (int i = 0; i < tail.size; ++i)
            {
                game_grid[tail.array[i].x][tail.array[i].y] = true;
            }
        }

        BeginDrawing();

        // HUD
        BeginScissorMode(0, 0, SC_WDTH, HUD_HGHT);
        ClearBackground(GREEN);
        DrawText(TextFormat("Blocks Collected: %i", score), SC_WDTH * 0.1,
                 HUD_HGHT * 0.4, 20, BLACK);
        DrawText(TextFormat("Highest Score: %i", high_score), SC_WDTH * 0.6,
                 HUD_HGHT * 0.4, 20, BLACK);
        DrawText(TextFormat("Use Arrow Keys To Move"), SC_WDTH * 0.35,
                 HUD_HGHT * 0.7, 20, BLACK);
        EndScissorMode();

        // game grid
        BeginScissorMode(0, HUD_HGHT, SC_WDTH, SC_HGHT - HUD_HGHT);
        ClearBackground(RAYWHITE);

        for (int i = 0; i < NO_BLCKS_ROW; ++i)
        {
            for (int j = 0; j < NO_BLCKS_COL; ++j)
            {
                if (game_grid[i][j])
                {
                    DrawRectangle(i * BLOCK_LENGTH,
                                  HUD_HGHT + j * BLOCK_LENGTH, BLOCK_LENGTH,
                                  BLOCK_LENGTH, DARKBLUE);
                }
            }
        }

        DrawRectangleLines(plyrPos.x * BLOCK_LENGTH,
                           HUD_HGHT + plyrPos.y * BLOCK_LENGTH,
                           BLOCK_LENGTH, BLOCK_LENGTH, DARKBROWN);
        DrawRectangle(tictac_pos.x * BLOCK_LENGTH,
                      HUD_HGHT + tictac_pos.y * BLOCK_LENGTH,
                      BLOCK_LENGTH, BLOCK_LENGTH, BLACK);

        EndScissorMode();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
