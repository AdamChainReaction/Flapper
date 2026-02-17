#include "objects.h"

const int nativeWidth = 640;
const int nativeHeight = 360;

// PLAYER 
void Player::Update(){
    if(alive){
        Move();
    }
}

void Player::Move(){
    pos.x = nativeWidth / 3;

    float gravity = 750;

    if(IsKeyPressed(KEY_SPACE)){
        velocity.y = -jumpForce;
    }
    else{
        velocity.y += gravity * dt;
    }

    pos.y += velocity.y * dt;
}

void Player::Draw(){
    DrawCircleV(pos, 12, playerColor);
}
//

// DOUBLE PIPE
void DoublePipe::Update(){
    recTop = {pos.x + width / 2, pos.y - 1000 - spacing / 2, width, 1000};
    recBottom = {pos.x + width / 2, pos.y + spacing / 2, width, 1000};
    Move();
    recTop = {pos.x + width / 2, pos.y - 1000 - spacing / 2, width, 1000};
    recBottom = {pos.x + width / 2, pos.y + spacing / 2, width, 1000};
}

void DoublePipe::Move(){
    pos.x -= speed * dt;
}

void DoublePipe::Draw(){

    DrawRectangle(recTop.x, recTop.y, recTop.width, recTop.height, pipeColor);
    DrawRectangle(recBottom.x, recBottom.y, recBottom.width, recBottom.height, pipeColor);

}
//
