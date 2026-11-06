#include "game.hh"

Game::Game(){
    this->score = 0;
    this->gameActive = false;
    this->gameID = 0;
    this->p1 = Player();
    this->p2 = Player();
}

Game::~Game(){}

bool Game::setScore(int s){
    if (s < 0) return false;
    this->score = s;
    return true;
}

int Game::getScore(){
    return this->score;
}

bool Game::update(){
    // update players
    // update ball
}

bool Game::updateBall(){

    // TODO: escape if stationary

    // x-axis
    if (ball.dir_right){
        ball.hitbox.x += ball.velocity.x;
    }
    else{
        ball.hitbox.x -= ball.velocity.x;
    }

    // y-axis
    if (ball.dir_up){
        ball.hitbox.y += ball.velocity.y;
    }
    else{
        ball.hitbox.y -= ball.velocity.y;
    }

}

Vector2 Game::getBallPos(){
    return (Vector2){this->ball.hitbox.x, this->ball.hitbox.y};
}

bool Game::checkCollision(Player p){

    Hitbox paddle = p.getHitbox();
    Vector2 pos = getBallPos();

    if (pos.y >= paddle.y && pos.y <= (paddle.y + paddle.height)){
        // y collision
        
        if (ball.dir_right){
            if (pos.x >= paddle.x && pos.x <= (paddle.x + paddle.width)){
                return true;    // x collision (going right)
            }
        }
        else{
            if (pos.x <= paddle.x && pos.x >= (paddle.x - paddle.width)){
                return true;    // x collision (going left)
            }
        }
    }

    // no collision
    return false;
}