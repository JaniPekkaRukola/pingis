#include "player.hh"

// Player::Player() : score(0), posX(0), posY(0){};
Player::Player(){
    this->score = 0;
    this->hitbox.x = 0;
    this->hitbox.y = 0;
    this->hitbox.width = 2;
    this->hitbox.height = 5;

    // this->posX = 0;
    // this->posY = 0;
    // this->width = 2;
}
Player::~Player(){}

bool Player::setId(int id){
    // TODO: do unique id system (uuid)
    this->id = id;
}

// TODO: out of bounds check for these
void Player::setPosX(int x) { this->hitbox.x = x; }
void Player::setPosY(int y) { this->hitbox.y = y; }

int Player::getPosX() { return this->hitbox.x; }
int Player::getPosY() { return this->hitbox.y; }
Hitbox Player::getHitbox() { return this->hitbox; }