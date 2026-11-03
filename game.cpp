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