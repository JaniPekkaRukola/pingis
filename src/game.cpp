#include "game.hh"

Game::Game(){
    this->score = 0;
    this->gameActive = false;
    this->gameID = 0;

    // these needed?
    // this->p1 = Player();
    // this->p2 = Player();
    // this->ball = Ball();

    // this->p1.name = "p1";
    // this->p2.name = "p2";

    this->p1.setPosX(1);
    this->p1.setWidth(2);
    this->p1.setHeight(10);
    this->p1.sensor = A0;

    this->p2.setPosX(SCREEN_WIDTH - 1);
    this->p2.setWidth(2);
    this->p2.setHeight(10);
    this->p1.sensor = A1;

    this->ball.setSize(1);
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

    // TODO: collision stuff
    // logic: if collision: 
    //          - change ball dir
    //          - calculate new new ball velocity (x and y) based on friction, paddle & ball velocity, ball angle?
    // else: score++, ball reset
    return true;
}

bool Game::draw(){
    // draw players
    p1.draw();
    p2.draw();

    // draw ball
    ball.draw();
    return true;
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

