/*******************************************************************************************
*
*   raylib game template
*
*   <Game title>
*   <Game description>
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2021 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <stdio.h> // NOTE: Declares global (extern) variables and screens functions


#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
// NOTE: Those variables are shared between modules through screens.h
//----------------------------------------------------------------------------------

typedef struct  
{
    Vector2 position;
    Color color;
    Texture2D texture;
    Vector2 speed;
    Vector2 size;
} GameObject;
Font font = { 0 };
Music music = { 0 };
Sound fxCoin = { 0 };



//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 450;

// Required variables to manage screen transitions (fade-in, fade-out)
static float transAlpha = 0.0f;
static bool onTransition = false;
static bool transFadeOut = false;
static int transFromScreen = -1;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void ChangeToScreen(int screen);     // Change to screen, no transition effect

static void TransitionToScreen(int screen); // Request transition to next screen
static void UpdateTransition(void);         // Update transition effect
static void DrawTransition(void);           // Draw transition effect (full-screen rectangle)
static void SnakeOver(void);
static void UpdateDrawFrame(void);      
static bool CheckCollision(GameObject, GameObject);
bool playSnake = true, playPong = true;
bool gameOver = false;

    // Update and draw one frame

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------




GameObject bar1, bar2, fruit, ball;
GameObject snake[500]; //cada parte da cobra vai ser um elemento da array
Vector2 snakePos[500];
int snakeLenght = 1;
int speed = 5;
bool allowMove = 1;
Vector2 offset = {0};
int frameCounter = 0;
static int points = 0;

int main(void)
{

    
    // Initialization
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "SNONG");
    fruit.position.x = screenWidth/2;
    fruit.position.y = screenHeight/2;
    bar1.position.x = 5;
    bar1.position.y = screenHeight/2;
    bar2.position.x = screenWidth - 12;
    bar2.position.y = screenHeight/2;
    bar1.color = WHITE;
    bar2.color = WHITE;
    ball.speed = (Vector2){200,200};
    ball.size = (Vector2){20, 20};
    ball.color = WHITE;
    ball.position = (Vector2){50, 50};
    fruit.size = (Vector2){25,25};
    offset = (Vector2){screenWidth%15, screenHeight%15};
    bar1.size = (Vector2){5, 50};
    bar2.size = (Vector2){5, 50};
    InitAudioDevice();      // Initialize audio device
    for(int i = 0; i < 256; i++){
        snake[i].position = (Vector2){offset.x/2, offset.y/2};
        snake[i].speed = (Vector2) {speed, 0};
        snakePos[i] = (Vector2) {0,0};
        snake[i].color = GREEN;
        snake[i].size = (Vector2){25,25};
    }

    // Load global data (assets that must be available in all screens, i.e. font)
    font = LoadFont("resources/mecha.png");
    music = LoadMusicStream("resources/ambient.ogg");
    fxCoin = LoadSound("resources/coin.wav");

    SetTargetFPS(60);       // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {  

        if(!playSnake && !playPong){
            int start = frameCounter;
            while(start - frameCounter > -700) { //delay antes de acabar
                frameCounter++;
                printf("%d\n", start - frameCounter);
            }
            gameOver = true;
        }
        if(IsKeyPressed(KEY_R)) gameOver = true;
        bar1.position.y = GetMousePosition().y;
        if((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && snake[0].speed.x == 0 && allowMove) {
            snake[0].speed = (Vector2){speed, 0};
            allowMove = false;
        }
        if((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && snake[0].speed.x == 0 && allowMove) {
            snake[0].speed = (Vector2){-speed, 0};
            allowMove = false;
        }
        if((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && snake[0].speed.y == 0 && allowMove) {
            snake[0].speed = (Vector2){0, speed};
            allowMove = false;
        }
        if((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && snake[0].speed.y == 0 && allowMove) {
            snake[0].speed = (Vector2){0, -speed};
            allowMove = false;
        }

        //colisao com ela msm
        for(int i = 1; i < snakeLenght; i++){
            if(snake[0].position.x == snake[i].position.x && snake[0].position.y == snake[i].position.y) {
                SnakeOver();
            }
        }

    
    if(CheckCollision(bar2, ball)){
        ball.speed = (Vector2){-GetRandomValue(200, 600), -GetRandomValue(200, 600)};

    }


    if(snake[0].position.x >= screenWidth - 20 || snake[0].position.x <= -10) SnakeOver();
    if(snake[0].position.y >= screenHeight - 20 || snake[0].position.y <= -5) SnakeOver();


    //colisao com a comida
    if(CheckCollision(snake[0], fruit)){
            fruit.position = (Vector2){GetRandomValue(0, screenWidth - 40), GetRandomValue(0, screenHeight - 40)};
            snakeLenght++; 
            points++;
            snake[snakeLenght].position = snakePos[snakeLenght-1];
    }

        frameCounter++;
        // printf("%d\n", frameCounter);
        UpdateDrawFrame();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // Unload global data loaded
    UnloadFont(font);
    UnloadMusicStream(music);
    UnloadSound(fxCoin);
    CloseAudioDevice();     // Close audio context
    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}




// Update and draw game frame
static void UpdateDrawFrame(void)
{

   ball.position.x += ball.speed.x * GetFrameTime();
   ball.position.y += ball.speed.y * GetFrameTime();
   if(playSnake) {
        for (int i = 0; i < snakeLenght; i++) snakePos[i] = snake[i].position;
            for(int i = 0; i < snakeLenght; i++){
                if(i == 0) {
                    snake[0].position.x += snake[0].speed.x;
                    snake[0].position.y += snake[0].speed.y;
                    allowMove = true;
                } else snake[i].position = snakePos[i-1];
            }
   }

    //perder pong  
    if(ball.position.x <= -5) {
        //bar1.size = (Vector2){0,0};
        playPong = false;
    }
    if(ball.position.y >= screenHeight-20) ball.speed = (Vector2){ball.speed.x, -ball.speed.y};
    if(ball.position.y <= 0) ball.speed = (Vector2){ball.speed.x, -ball.speed.y};
    if(CheckCollision(ball, bar1)){
        ball.speed = (Vector2){GetRandomValue(200, 600), GetRandomValue(200, 600)};
        points++;
    }    

    bar2.position.y = ball.position.y;

    // Update
    //----------------------------------------------------------------------------------
    UpdateMusicStream(music);       // NOTE: Music keeps playing between screens

    // Draw
    //---------------------------------------------------------------------------------


    

    if(!gameOver) {
        BeginDrawing();
            ClearBackground(BLACK);
            fruit.color = RED;
            DrawRectangle(fruit.position.x, fruit.position.y, 25.0, 25.0, fruit.color);
            for(int i = 0; i < snakeLenght; i++) DrawRectangleV(snake[i].position, snake[i].size, snake[i].color);
            DrawRectangleV(bar1.position, bar1.size, bar1.color);
            DrawRectangleV(bar2.position, bar2.size, bar2.color);
            DrawRectangleV(ball.position, ball.size, ball.color);
            DrawText(TextFormat("%d", points), (screenWidth/2) - 7, 35, font.baseSize+5, ORANGE);
        EndDrawing();
    } else {
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText(TextFormat("End Score: %d", points), (screenWidth/2)-100, screenHeight/2, font.baseSize+10, RED);    
            DrawText("Game Over!", (screenWidth/2)-160, 70, font.baseSize+40, RED);    
            DrawText("Press 'Esc' or close window to quit", (screenWidth/2)-160, screenHeight - 100, font.baseSize+5, RED);    
        EndDrawing();
    }
    //----------------------------------------------------------------------------------
}



void SnakeOver(void){
    allowMove = false;
    if(playSnake) {
        for (int i = 0; i < snakeLenght; i++) {
            snake[i].speed = (Vector2){0, 0};
            allowMove = false;
            snake[i].color = MAROON;
        } 
        playSnake = 0;
        allowMove = 0;
    }
}


static bool CheckCollision(GameObject obj1, GameObject obj2){
        if ((obj1.position.x < (obj2.position.x + obj2.size.x) && (obj1.position.x + obj2.size.x) > obj2.position.x) &&
            (obj1.position.y < (obj2.position.y + obj2.size.y) && (obj1.position.y + obj2.size.y) > obj2.position.y)) {
            return true;
        }
        return false; 
}
