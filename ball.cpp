#include "ball.hh"

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

    // x-axis
    if (this->dir.right){
        this->hitbox.x += this->velocity.x;
    }
    else{
        this->hitbox.x -= this->velocity.x;
    }

    // y-axis
    if (this->dir.up){
        this->hitbox.y += this->velocity.y;
    }
    else{
        this->hitbox.y -= this->velocity.y;
    }
}

bool Ball::draw(){
    // TODO:
}