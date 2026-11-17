#include "ball.hh"
#include "struct.hh"

// this might fix display obj include issues
#include <Adafruit_SSD1306.h>
extern Adafruit_SSD1306 display; 

Ball::Ball(){
    this->hitbox.x = 0;
    this->hitbox.y = 0;
    this->velocity.x = 0;
    this->velocity.y = 0;
    this->friction = 0.1f;

    this->dir.right = false;
    this->dir.up = false;
    this->dir.stationary = true;
}

Ball::~Ball(){}

Vector2 Ball::getPos(){
    return (Vector2){this->hitbox.x, this->hitbox.y};
}

Dir Ball::getDir(){
    return this->dir;
}

bool Ball::update(){
    if (this->dir.stationary){
        // if (DEBUG) Serial.println("[DEBUG - @Ball::update()] ball stationary, returning");
        return;
    }

    // TODO: out of bounds check (using "SCREEN_WIDTH" and "SCREEN_HEIGHT")
    // NOTE: prolly wanna do future projection calculations for oob check instead of reverting later

    // x-axis
    if (this->dir.right){
        this->pos.x += this->velocity.x;
    }
    else{
        this->pos.x -= this->velocity.x;
    }

    // y-axis
    if (this->dir.up){
        this->pos.y += this->velocity.y;
    }
    else{
        this->pos.y -= this->velocity.y;
    }

    // update hitbox
    this->hitbox.x = this->pos.x;
    this->hitbox.y = this->pos.y;
}

bool Ball::draw(){
    drawRectHB(display, this->hitbox, WHITE);
}