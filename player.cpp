#include "player.hh"

// Player::Player() : score(0), posX(0), posY(0){};
Player::Player(){
    this->score = 0;
    this->posX = 0;
    this->posY = 0;
    this->width = 2;
}
Player::~Player(){}

bool Player::setId(int id){
    // TODO: do unique id system (uuid)
    this->id = id;
}

// TODO: out of bounds check for these
void Player::setPosX(int x) { this->posX = x; }
void Player::setPosY(int y) { this->posX = y; }

int Player::getPosX() { return this->posX; }
int Player::getPosY() { return this->posY; }