#include <raylib.h>
#include "raymath.h"
#include <stdio.h>
#include <string>
#include <vector>

class Player
{
    private:
    Vector2 velocity = {0};
    float jumpForce = 300;


    public:
    void Update();
    void Move();
    void Draw();

    float dt;
    Vector2 pos;

    Color playerColor = RED;

    bool alive = true;

};

class DoublePipe
{
    private:
    float width = 10;
    
    public:
    float spacing;
    DoublePipe(float spaceBetween, Vector2 initPos, float size){
        spacing = spaceBetween;
        width = size;
        pos = initPos;
    };

    void Update();
    void Move();
    void Draw();

    Vector2 pos = {0};
    float dt;
    float speed = 5;

    Rectangle recTop = { pos.x, pos.y, width, 1000 };
    Rectangle recBottom = { pos.x, pos.y, width, 1000 };

    Color pipeColor = GREEN;

    bool active = false;
    bool counted = true;
};
