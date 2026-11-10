#include "game.hh"

Game::Game(){
    this->score = 0;
    this->gameActive = false;
    this->gameID = 0;
    this->p1 = Player();
    this->p2 = Player();
    this->ball = Ball();
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
    this->p1.update();
    this->p2.update();

    // update ball
    this->ball.update();
}

bool Game::draw(){
    // draw players
    p1.draw();
    p2.draw();

    // draw ball
    ball.draw();
}

bool Game::checkCollision(Player p){

    Hitbox paddle = p.getHitbox();
    Vector2 pos = this->ball.getPos();
    Dir ballDir = this->ball.getDir();


    if (pos.y >= paddle.y && pos.y <= (paddle.y + paddle.height)){
        // y collision
        
        if (ballDir.right){
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