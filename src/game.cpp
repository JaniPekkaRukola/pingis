#include "game.hh"
#include "buzzer.hh"
#include <Arduino.h>
#include "struct.hh"

#include <Adafruit_SSD1306.h>
extern Adafruit_SSD1306 display;
extern void saveStatsEEPROM();
extern void saveMatchEEPROM(int p1Score, int p2Score);

// Game timer
volatile uint8_t game_time = 60; // seconds
volatile bool timeUp = false;

Game::Game(){
    this->gameActive = false;
    this->settings.drawScore = true;
    this->settings.increaseSpeed = true;
    
    // player 1 init
    this->p1.setPosX(this->playerOffset);
    this->p1.sensor = A0;
    this->p1.setId(1);
    this->p1_score = 0;
    
    // player 2 init
    this->p2.setPosX(SCREEN_WIDTH - this->playerOffset - this->p2.getHitbox().width);
    this->p2.sensor = A1;
    this->p2.setId(2);
    this->p2_score = 0;

    // ball init
    this->ball.setSize(2, 2);
    this->ball.setPos((SCREEN_WIDTH / 2) - (this->ball.getSize().x / 2), (SCREEN_HEIGHT / 2) - (this->ball.getSize().y / 2));
}

Game::~Game(){}

void Game::startGame(){
    this->p1_score = 0;
    this->p2_score = 0;
    this->ball.setPos((SCREEN_WIDTH / 2) - (this->ball.getSize().x / 2), (SCREEN_HEIGHT / 2) - (this->ball.getSize().y / 2));

    uint8_t randomDir = random(0,2);
    this->ball.setDir(randomDir == 1, false, false);
    this->ball.setVelocity({this->settings.ballSpeed, 0.0});

    this->stats.gamesPlayed++;

    this->gameActive = true;

    this->startGameTimer();
    this->sendScoreUART("START");
    if (DEBUG) Serial.println(F("Game starting"));
}

void Game::stopGame(){
    this->ball.setDir(false, false, true);
    this->gameActive = false;
    TIMSK1 &= ~(1 << OCIE1A); // dont compare timer interrupt anymore

    this->sendScoreUART("END");
    saveStatsEEPROM();
    saveMatchEEPROM(this->p1_score, this->p2_score);

    p1.reset();
    p2.reset();

    if (DEBUG) Serial.println(F("Game stopping"));
}

void Game::sendScoreUART(const char* event){
    Serial.print(F("SCORE:"));
    Serial.print(event);
    Serial.print(',');
    Serial.print(this->p1_score);
    Serial.print(',');
    Serial.println(this->p2_score);
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
            if (this->settings.enableAudio) Buzzer::play(Buzzer::PADDLE);
            this->ball.setDirX(true);
            // TODO: calculate new velocity for ball x and y
        }

        // ball out of bounds (player 1 missed) -> player 2 scores
        else if (this->ball.getPos().x < 0){
            this->p2_score++;
            this->stats.totalGoals++;
            this->sendScoreUART("UPDATE");
            if (this->settings.enableAudio) Buzzer::play(Buzzer::SCORE);
            this->ball.toggleDirX();
            roundReset();
            if (DEBUG) Serial.println(F("PLAYER 2 SCORES!"));
        }
    }

    // player 2 (going right)
    else{

        // check ball collision with player 2
        if (checkCollision(this->p2)){
            if (this->settings.enableAudio) Buzzer::play(Buzzer::PADDLE);
            this->ball.setDirX(false);
            // TODO: calculate new velocity for ball x and y
        }

        // ball out of bounds (player 2 missed) -> player 1 scores
        else if (this->ball.getPos().x > SCREEN_WIDTH){
            this->p1_score++;
            this->stats.totalGoals++;
            this->sendScoreUART("UPDATE");
            if (this->settings.enableAudio) Buzzer::play(Buzzer::SCORE);
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

    if (this->settings.increaseSpeed){
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
    display.setCursor(58,2);
    display.print(game_time);

    // if (this->drawScore){
    if (this->settings.drawScore){
        // TODO: shift score to the left when double digits
        // TODO: make sure score is actually in the middle (is textwidth correct?)
        int textWidth = charSize_1.x;
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

void Game::gameTimerInitialization() {
    uint8_t old_sreg = SREG;
    cli();
    TIMSK1 = 0;
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;

    OCR1A = 15624; // from output frequency formula
    TCCR1B |= (1 << WGM12) | (1 << CS10) | (1 << CS12); // CTC mode and 1024 prescaler
    SREG = old_sreg;
};

void Game::startGameTimer() {
    uint8_t old_sreg = SREG;
    cli();
    TCNT1 = 0;
    game_time = 60;
    timeUp = false;
    TIFR1 = (1 << OCF1A); // clear interrupt flag (if there would be old one hanging)
    TIMSK1 = (1 << OCIE1A); // to compare interrupt
    SREG = old_sreg;
}

bool Game::isTimeUp() {
    return timeUp;
}

ISR(TIMER1_COMPA_vect) {
    if (game_time > 0) {
        game_time--;
    } else {
        timeUp = true;
    }
}