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

    if (ball.dir == dir_nil) return;


    // x-axis
    if (ball.dir == dir_left){
        ball.pos.x -= ball.velocity.x;
    }
    if (ball.dir == dir_right){
        ball.pos.x += ball.velocity.x;
    }

    // y-axis
    // ball.pos.y

}