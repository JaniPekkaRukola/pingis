#include "player.hh"

#include <Adafruit_SSD1306.h>
extern Adafruit_SSD1306 display;

int getDistance(uint8_t sensor);

Player::Player(){
    this->pos.x = 0;
    this->pos.y = 0;

    this->hitbox.x = this->pos.x;
    this->hitbox.y = this->pos.y;
    this->hitbox.width = 2;
    this->hitbox.height = 5;
}

Player::~Player(){}

bool Player::setId(int id){
    // TODO: do unique id system (uuid)
    this->id = id;
    return true;
}

bool Player::update(){

    // get distance
    int dist = getDistance(this->sensor);

    //TODO: smooth out the distance
    //eg: if new distance is > accepted variation from previous -> discard

    // invert distance (since [0,0] is topleft)
    dist = SCREEN_HEIGHT - dist;

    if (dist > 100.0f) return true;

    // out of bound top
    if (dist < 0) dist = 0;
    
    // out of bound bottom
    if (dist + this->hitbox.height > SCREEN_HEIGHT){
        dist = SCREEN_HEIGHT - this->hitbox.height;
    }

    // set position
    this->pos.y = dist;

    // TODO: make this better
    // append locationhistory 
    // appendArr(&this->locationHistory, dist);
    locationHistory[2] = locationHistory[1];
    locationHistory[1] = locationHistory[0];
    locationHistory[0] = dist;

    // update hitbox
    this->hitbox.x = this->pos.x;
    this->hitbox.y = this->pos.y;

    return true;
}

bool Player::draw(){
    drawRectHB(display, this->hitbox, WHITE);
    return true;
}

void Player::setPosX(int x) {
    // TODO: out of bounds check
    this->pos.x = x;
    this->hitbox.x = this->pos.x;
}

void Player::setPosY(int y){
    // TODO: out of bounds check
    this->pos.y = y;
    this->hitbox.y = this->pos.y;
}

bool Player::setWidth(int w){
    if (w <= 0) return false;
    this->hitbox.width = w;
    return true;
}
bool Player::setHeight(int h){
    if (h <= 0) return false;
    this->hitbox.height = h;
    return true;
}

int Player::getPosX() { return this->pos.x; }
int Player::getPosY() { return this->pos.y; }
Hitbox Player::getHitbox() { return this->hitbox; }

void Player::calculateVelocity(){
    // TODO: calculate palyers velocity with "velocitySampleCount" and "locationHistory" array
    // and update this->velocity.x and y
}