#include "ball.hh"
#include "struct.hh"

// this might fix display obj include issues
#include <Adafruit_SSD1306.h>
extern Adafruit_SSD1306 display; 

Ball::Ball(){
    this->hitbox.x = 0;
    this->hitbox.y = 0;
    this->hitbox.width = 1;
    this->hitbox.height = 1;

    this->velocity.x = 0;
    this->velocity.y = 0;
    this->friction = 0.1f;

    this->dir.right = false;
    this->dir.up = false;
    this->dir.stationary = true;

}

Ball::~Ball(){}


bool Ball::setDir(bool right, bool up, bool stationary){ // this could be void since dir dont have validation
    this->dir.right = right;
    this->dir.up = up;
    this->dir.stationary = stationary;
    return true;
}

Dir Ball::getDir(){
    return this->dir;
}

bool Ball::draw(){
    drawRectHB(display, this->hitbox, WHITE);
    return true;
}


bool Ball::update(){
    if (this->dir.stationary){
        // if (DEBUG) Serial.println("[DEBUG - @Ball::update()] ball stationary, returning");
        return false;
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
    this->updateHitbox();
    return true;
}

void Ball::updateHitbox(){
    this->hitbox.x = this->pos.x;
    this->hitbox.y = this->pos.y;
}


bool Ball::setSize(int w, int h){
    if (w <= 0 || h <= 0) return false;
    if (w != h){ Serial.println("[WARNING]: ball size is not uniform"); }
    this->hitbox.width = w;
    this->hitbox.height = h;
    return true;
}

Vector2 Ball::getSize(){
    return (Vector2){this->hitbox.width, this->hitbox.height};
}

bool Ball::setPos(float x, float y){

    Serial.println("Ball pos x set = " + String(x));
    Serial.println("Ball pos y set = " + String(y));

    //TODO: out of bounds check here
    this->pos.x = x;
    this->pos.y = y;

    this->updateHitbox();

    return true;
}

Vector2 Ball::getPos(){
    return (Vector2){this->pos.x, this->pos.y};
}

bool Ball::setVelocity(Vector2 v){
    if (v.x < 0 || v.y < 0) return false;
    this->velocity = v;
    return true;
}

Vector2 Ball::getVelocity(){
    return this->velocity;
}