#include "objects.h"
#include <string>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

std::vector<DoublePipe> pipes;
float spawnTimer = 0.0f;

float spawnFreq = 1.0f;

unsigned int score = 0;

const int nativeWidth = 640;
const int nativeHeight = 360;

void Spawner(float dt, float spawnSpeed){

    spawnTimer += dt;

    if(spawnTimer >= spawnFreq){
        spawnTimer = 0.0f;

        for (size_t i = 0; i < pipes.size(); i++)
        {
            if (!pipes[i].active)
            {
                float deviation = GetRandomValue(pipes[i].spacing + 5, nativeHeight - pipes[i].spacing - 5);
                Vector2 spawnPoint = {nativeWidth + pipes[i].recBottom.width, deviation};

                pipes[i].active = true;
                pipes[i].pos = spawnPoint;
                pipes[i].speed = spawnSpeed;
                pipes[i].counted = false;
                break;
            }
        }
    }
}

void UpdatePipes(float dt, float speed, Color pipeColor){
    for (int i = 0; i < (int)pipes.size(); i++)
    {
        if(pipes[i].active){
            pipes[i].pipeColor = pipeColor;

            if(!pipes[i].counted && pipes[i].pos.x <= nativeWidth / 3 - pipes[i].recBottom.width / 2){
                pipes[i].counted = true;
                score++;
            }

            if(pipes[i].pos.x <= -pipes[i].recBottom.width * 2){
                pipes[i].active = false;
            }
            else
            {
                pipes[i].dt = dt;
                pipes[i].speed = speed;
                pipes[i].Update();
            }

        }
    }
}

void DrawPipes(){
    for (size_t i = 0; i < pipes.size(); i++)
    {
        if(pipes[i].active){
            pipes[i].Draw();
        }
    }
}

Rectangle GetVirtualScreenRect(){
    float scale = fminf(
        (float)GetScreenWidth()  / nativeWidth,
        (float)GetScreenHeight() / nativeHeight
    );

    float width  = nativeWidth  * scale;
    float height = nativeHeight * scale;

    float x = (GetScreenWidth()  - width)  * 0.5f;
    float y = (GetScreenHeight() - height) * 0.5f;

    return { x, y, width, height };
}

void reset(Player &p){
    spawnTimer = 0.0f;
    p.alive = true;
    p.pos.y = nativeHeight / 2;
    score = 0;
    for (size_t i = 0; i < pipes.size(); i++){
        pipes[i].active = false;
        pipes[i].counted = false;
        pipes[i].speed = 0;
        pipes[i].pos = {-100,0};
    }
}

void SetGuiStyleDefaults(){
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 64);
    GuiSetStyle(LABEL, TEXT_SIZE, 128);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 5);
    
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(GREEN));
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(DARKGREEN));
    
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(DARKGREEN));
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(GREEN));
    
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(DARKGREEN));
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(GREEN));

    //
    GuiSetStyle(COMBOBOX, BORDER_WIDTH, 5);
    GuiSetStyle(COMBOBOX, BASE_COLOR_NORMAL, ColorToInt(GREEN));
    GuiSetStyle(COMBOBOX, BASE_COLOR_FOCUSED, ColorToInt(DARKGREEN));
    
    GuiSetStyle(COMBOBOX, TEXT_COLOR_NORMAL, ColorToInt(DARKGREEN));
    GuiSetStyle(COMBOBOX, TEXT_COLOR_FOCUSED, ColorToInt(GREEN));
    
    GuiSetStyle(COMBOBOX, BORDER_COLOR_NORMAL, ColorToInt(DARKGREEN));
    GuiSetStyle(COMBOBOX, BORDER_COLOR_FOCUSED, ColorToInt(GREEN));
    
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(GREEN));
}

int main()
{
    //================================== UI AND INITIALS ===========================================//
    const Image icon = LoadImage("./assets/icon.png");

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(nativeWidth, nativeHeight, "lapper");
    SetWindowIcon(icon);
    float maxFps = 12;
    SetTargetFPS((int)maxFps * 5);
    MaximizeWindow();

    SetGuiStyleDefaults();

    Font font = LoadFontEx("./assets/font.ttf", 64, NULL, NULL);
    GuiSetFont(font);

    Color playerColor = RED;
    Color pipeColor = GREEN;
    Color bgColor = BLACK;
    bool showFPS = false;

    RenderTexture2D target = LoadRenderTexture(nativeWidth, nativeHeight);
    
    const int POOL_SIZE = 24;
    
    const float maxPipeSpeed = 600;
    float pipeSpeed = 200;
    
    Player player;
    player.pos.y = nativeHeight / 2;
    
    std::string const gameStates[4] =  {"menu", "game", "dead", "settings"};
    std::string activeState = gameStates[0];
    //=============================================================================================//
    
    //======================================= SHADERS =============================================//
    Shader styler = LoadShader(0, "./assets/styler.fs");
    //=============================================================================================//

    // Filling Pipe Object Pool
    for (int i = 0; i < POOL_SIZE; i++)
    {
        DoublePipe p(130, {-100, 0}, 50);
        p.speed = 0;
        p.active = false;

        pipes.push_back(p);
    }
    
    // MAIN LOOP
    while (!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_F11)) ToggleFullscreen();


        //======================================= Game Logic =======================================//
        if(IsWindowFocused()){
            if(activeState == gameStates[1]){
                // Delta Time
                float delta = GetFrameTime();

                // Global Update
                if(player.alive){
                    player.playerColor = playerColor;
                    pipeSpeed >= maxPipeSpeed ? pipeSpeed = maxPipeSpeed : pipeSpeed += 5.0f * delta;
                    spawnFreq = 1.4f / (pipeSpeed / 200);

                    Spawner(delta, pipeSpeed);

                    player.dt = delta;
                    player.Update();
                    UpdatePipes(delta, pipeSpeed, pipeColor);

                    for (int i = 0; i < POOL_SIZE; i++)
                    {
                        if (!pipes[i].active) continue;
                        if(CheckCollisionCircleRec(player.pos, 12, pipes[i].recBottom) || CheckCollisionCircleRec(player.pos, 12, pipes[i].recTop)){
                            player.alive = false;
                            activeState = gameStates[2];
                        }
                    }
                }

                // Render Texture Drawing
                BeginTextureMode(target);
                    ClearBackground(bgColor);

                    player.Draw();
                    DrawPipes();

                    DrawTextEx(font, TextFormat("Score: %d", score), {0, nativeHeight - 32}, 32, 1, RED);
                EndTextureMode();

            }
        }
        //========================================================================================//
        
        //======================================= Drawing =======================================//
        BeginDrawing();
            ClearBackground(BLACK);
            if(showFPS) DrawFPS(0,0);

            if(activeState == gameStates[0]){ /* Main Menu */
                float sw = (float)GetScreenWidth();
                float sh = (float)GetScreenHeight();

                DrawRectangleGradientV(0, sh / 2, sw, sh / 2, BLACK, GREEN);

                GuiLabel({0,-250, sw, sh}, "- - F l a p p e r - -");

                if (GuiButton((Rectangle){sw / 2 - 150, sh / 2, 300, 100}, "P l a y"))
                {
                    activeState = gameStates[1];
                }

                if (GuiButton((Rectangle){sw / 2 - 150, sh - sh / 3, 300, 100}, "S e t t i n g s"))
                {
                    activeState = gameStates[3];
                }

                if (GuiButton((Rectangle){sw / 2 - 150, sh - sh / 3 + 165, 300, 100}, "E x i t"))
                {
                    break;
                }
            }
            else if(activeState == gameStates[1]){ /* Main Game */
                Rectangle source = {
                    0.0f,
                    0.0f,
                    (float)target.texture.width,
                    -(float)target.texture.height
                };

                Rectangle dest = GetVirtualScreenRect();

                BeginShaderMode(styler);
                    DrawTexturePro(
                        target.texture,
                        source,
                        dest,
                        { 0.0f, 0.0f },
                        0.0f,
                        WHITE
                    );
                EndShaderMode();
            }
            else if(activeState == gameStates[2]){ /* Death Screen */
                float sw = (float)GetScreenWidth();
                float sh = (float)GetScreenHeight();

                DrawRectangleGradientV(0, sh / 2, sw, sh / 2, BLACK, GREEN);

                GuiLabel({0,-250, sw, sh}, "x  Y o u  D i e d  x");
                GuiLabel({0,-100, sw, sh}, TextFormat("Final score: %d", score));
        
                if (GuiButton((Rectangle){sw / 2 - 150, sh / 2, 300, 100}, "R e s t a r t"))
                {
                    pipeSpeed = 200;
                    reset(player);
                    activeState = gameStates[1];
                }

                if (GuiButton((Rectangle){sw / 2 - 125, sh / 2 + 150, 250, 75}, "M e n u"))
                {
                    pipeSpeed = 200;
                    reset(player);
                    activeState = gameStates[0];
                }
            }
            else{ /* Settings Menu */
                SetTargetFPS((int)maxFps * 5);
                float sw = (float)GetScreenWidth();
                float sh = (float)GetScreenHeight();

                DrawRectangleGradientV(0, sh / 2, sw, sh / 2, BLACK, GREEN);

                GuiLabel({0,-250, sw, sh}, "- - S e t t i n g s - -");

                GuiLabel({0,-100, sw, sh}, "Pipes, Player  Background Colors");
                GuiColorPicker({sw / 2 - 50, sh / 2 - 50, 100, 100}, "Player Color", &playerColor);
                GuiColorPicker({sw / 2 - 250, sh / 2 - 50, 100, 100}, "Pipe Color", &pipeColor);
                GuiColorPicker({sw / 2 + 150, sh / 2 - 50, 100, 100}, "Background", &bgColor);

                GuiSliderBar({sw / 2, sh - sh/3, 100, 10},"Max FPS", TextFormat("%d", (int)maxFps * 5), &maxFps, 1, 51);

                GuiCheckBox({sw / 2 - 125, sh - 250, 50, 50}, "Show FPS", &showFPS);

                if (GuiButton((Rectangle){10, 10, 50, 50}, "<")) activeState = gameStates[0];
            }
        EndDrawing();
        //========================================================================================//
    }

    UnloadRenderTexture(target);
    UnloadFont(font);
    UnloadImage(icon);
    CloseWindow();
}
