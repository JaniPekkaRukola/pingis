#include "game.hh"
#include "buzzer.h"
#include <Arduino.h>
#include "struct.hh"

#include <Adafruit_SSD1306.h>
extern Adafruit_SSD1306 display;

Game::Game(){
    this->gameActive = false;
    this->gameID = 0;
    this->playerOffset = 3;
    this->drawScore = true;
    this->increaseSpeed = true;
    
    // player 1 init
    this->p1.setWidth(this->paddleSize.x);
    this->p1.setHeight(this->paddleSize.y);
    this->p1.setPosX(playerOffset);
    this->p1.sensor = A0;
    this->p1.setId(1); // TODO uuid
    this->p1_score = 0;
    // this->p1.name = "p1";

    
    // player 2 init
    this->p2.setWidth(this->paddleSize.x);
    this->p2.setHeight(this->paddleSize.y);
    this->p2.setPosX(SCREEN_WIDTH - playerOffset - this->p2.getHitbox().width);
    this->p2.sensor = A1;
    this->p2.setId(2); // TODO uuid
    this->p2_score = 0;
    // this->p2.name = "p2";


    // ball init
    this->ball.setSize(2, 2);
    this->ball.setPos((SCREEN_WIDTH / 2) - (this->ball.getSize().x / 2), (SCREEN_HEIGHT / 2) - (this->ball.getSize().y / 2));
}

Game::~Game(){}

void Game::startGame(){
    // TODO: randomize starting dir, or let the loser start when not first round?
    this->ball.setDir(true, false, false);
    this->ball.setVelocity({1.0, 0.0});

    this->gameActive = true;

    if (DEBUG) Serial.println(F("Game starting"));
}

void Game::stopGame(){
    this->ball.setDir(false, false, true);
    this->gameActive = false;
    if (DEBUG) Serial.println(F("Game stopping"));
}

// bool Game::setScore(int s){
//     if (s < 0) return false;
//     this->score = s;
//     return true;
// }

// int Game::getScore(){
//     return this->score;
// }

bool Game::update(){

    // if (p1_score >= winScore){
    //     gameActive = false;
    //     return true;
    // }
    // if (p2_score >= winScore){
    //     gameActive = false;
    //     return true;
    // }

    // update players
    this->p1.update();
    this->p2.update();

    // update ball
    this->ball.update();

    // Collision

    // player 1 (going left)
    if (!this->ball.getDir().right){
        
        // check ball collision with player 1
        if (checkCollision(this->p1)){
            if (this->enableAudio) Buzzer::play(Buzzer::PADDLE);
            this->ball.setDirX(true);
            // TODO: calculate new velocity for ball x and y
        }

        // ball out of bounds (player 1 missed) -> player 2 scores
        else if (this->ball.getPos().x < 0){
            this->p2_score++;
            if (this->enableAudio) Buzzer::play(Buzzer::SCORE);
            this->ball.toggleDirX();
            roundReset();
            if (DEBUG) Serial.println(F("PLAYER 2 SCORES!"));
        }
    }

    // player 2 (going right)
    else{

        // check ball collision with player 2
        if (checkCollision(this->p2)){
            if (this->enableAudio) Buzzer::play(Buzzer::PADDLE);
            this->ball.setDirX(false);
            // TODO: calculate new velocity for ball x and y
        }

        // ball out of bounds (player 2 missed) -> player 1 scores
        else if (this->ball.getPos().x > SCREEN_WIDTH){
            this->p1_score++;
            if (this->enableAudio) Buzzer::play(Buzzer::SCORE);
            this->ball.toggleDirX();
            roundReset();
            if (DEBUG) Serial.println(F("PLAYER 1 SCORES!"));
        }
    }

    return true;
}

void Game::roundReset(){
    this->ball.setPos((SCREEN_WIDTH / 2) - (this->ball.getSize().x / 2), (SCREEN_HEIGHT / 2) - (this->ball.getSize().y / 2));

    if (DEBUG) Serial.println(F("round reset"));

    if (this->increaseSpeed){
        this->ball.increaseVelocity({0.1, 0.0});
    }

    // TODO: delay round start for like a second
}

void Game::draw(){
    // draw players
    p1.draw();
    p2.draw();

    // draw ball
    ball.draw();
}

void Game::drawUI(){
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);

    if (this->drawScore){
        // TODO: shift score to the left when double digits
        // TODO: make sure score is actually in the middle (is textwidth correct?)
        int textWidth = 5;
        int offset = 5;
        int text_pos_y = 9;

        display.setCursor((SCREEN_WIDTH / 2) - textWidth - offset, text_pos_y);
        display.print(this->p1_score);
        
        display.setCursor((SCREEN_WIDTH / 2) + textWidth + offset, text_pos_y);
        display.print(this->p2_score);
    }
}

bool Game::checkCollision(Player p){

    Hitbox paddle = p.getHitbox();
    Vector2 pos = this->ball.getPos();
    Dir ballDir = this->ball.getDir();


    if (pos.y >= paddle.y && pos.y <= (paddle.y + paddle.height)){
        // y collision
        
        if (ballDir.right){
            if (pos.x >= paddle.x && pos.x <= (paddle.x + paddle.width)){
                if (DEBUG) Serial.println(F("Collision with p2"));
                return true;    // x collision (going right)
            }
        }
        else{
            if (pos.x <= paddle.x && pos.x >= (paddle.x - paddle.width)){
                if (DEBUG) Serial.println(F("Collision with p1"));
                return true;    // x collision (going left)
            }
        }
    }

    // no collision
    return false;
}

