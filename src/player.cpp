#include "player.hh"

// Player::Player() : score(0), posX(0), posY(0){};
Player::Player(){
    this->score = 0;
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
}

bool Player::update(){
    // TODO: 

    // logic:
    // get sensor distance
    // out of bounds check (using "SCREEN_HEIGHT")
    // set position
    // append history vector
    // update hitbox


    // TODO: append distance values to "locationHistory" vector
    // TODO: maintain vector so it wont grow too big


    // update hitbox
    this->hitbox.x = this->pos.x;
    this->hitbox.y = this->pos.y;
}

bool Player::draw(){
    drawRectHB(this->hitbox, WHITE);
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

int Player::getPosX() { return this->pos.x; }
int Player::getPosY() { return this->pos.y; }
Hitbox Player::getHitbox() { return this->hitbox; }

void Player::calculateVelocity(){
    // TODO: calculate palyers velocity with "velocitySampleCount" and "locationHistory" vector
    // and update this->velocity.x and y
}