#include "player.hh"

#include <Adafruit_SSD1306.h>
extern Adafruit_SSD1306 display;
extern unsigned long delta_t;

int getDistance(uint8_t sensor);

Player::Player(){
    this->pos.x = 0;
    this->pos.y = 0;
    this->velocity = 0.0f;
    this->id = 0;

    this->hitbox.x = this->pos.x;
    this->hitbox.y = this->pos.y;
    this->hitbox.width = this->paddleSize.x;
    this->hitbox.height = this->paddleSize.y;
}

Player::~Player(){}

int Player::getPosX() { return this->pos.x; }
int Player::getPosY() { return this->pos.y; }
Hitbox Player::getHitbox() { return this->hitbox; }
float Player::getVelocity(){ return this->velocity; }
void Player::setId(int id){ this->id = id; }

void Player::setPosX(int x){
    // TODO: out of bounds check
    this->pos.x = x;
    this->hitbox.x = this->pos.x;
}
void Player::setPosY(int y){
    // TODO: out of bounds check
    this->pos.y = y;
    this->hitbox.y = this->pos.y;
}
bool Player::setWidth(int w){
    if (w <= 0) return false;
    this->paddleSize.x = w;
    this->hitbox.width = w;
    return true;
}
bool Player::setHeight(int h){
    if (h <= 0) return false;
    this->paddleSize.y = h;
    this->hitbox.height = h;
    return true;
}

/*
bool Player::update_old(){

    // TODO: remove this
    if (this->id == 2) return false;

    // get distance
    int dist = getDistance(this->sensor);

    // Serial.println("Dist P1 = " + String(dist));
    // Serial.println("Prev Dist P1 = " + String(this->previousDistance));

    // check if distance is in accepted limits
    // if (!acceptMovement(dist)){
    //     Serial.println("acceptmovement returned false");
        // Serial.println("previous dist = ")
        // return false;
    // }



    // if (abs(dist - this->previousDistance) > this->maxMoveDelta){
    if (locationHistory[0] == 0){
        Serial.println("First round");
    }
    else if (abs(dist - locationHistory[0]) > this->maxMoveDelta){
        Serial.println("moved too quickly, movement disregarded");
        Serial.println("Dist = " + String(dist) + "  locathist[0] = " + String(locationHistory[0]));
        Serial.println("velocity = " + String(this->velocity));
        return false;
    }
    else if (dist > this->maxDistance){
        Serial.println("distance too far, movement disregarded");
        Serial.println("Dist = " + String(dist));
        return false;
    }



    // invert distance (since [0,0] is topleft)
    dist = SCREEN_HEIGHT - dist;

    // out of bounds - top
    if (dist < 0) dist = 0;
    
    // out of bounds - bottom
    if (dist + this->hitbox.height > SCREEN_HEIGHT){
        dist = SCREEN_HEIGHT - this->hitbox.height;
    }

    // set position
    this->pos.y = dist;

    // this->previousDistance = dist;

    // TODO: make this better
    // append locationhistory 
    // appendArr(&this->locationHistory, dist);
    locationHistory[2] = locationHistory[1];
    locationHistory[1] = locationHistory[0];
    locationHistory[0] = dist;

    calculateVelocity();

    // update hitbox
    this->hitbox.x = this->pos.x;
    this->hitbox.y = this->pos.y;

    return true;
}*/

bool Player::draw(){
    drawRectHB(display, this->hitbox, WHITE);
    return true;
}

void Player::reset(){
    // resets some player vars after a match
    this->velocity = 0.0f;
    for (int i = 0; i < velocitySampleCount; i++){ this->locationHistory[i] = 0.0f; }
}

void Player::calculateVelocity(){
    // calculates approximated velocity fast without using timers
    float v = 0.0f;
    for (int i = 0; i < velocitySampleCount; i++){
        v += locationHistory[i];
    }
    v /= (float)velocitySampleCount;
    this->velocity = abs(v - locationHistory[0]);
    if (this->velocity <= 0) this->velocity = 0.0f; // prevent weird -0.0f value
    
    // Serial.println("P1 v[]: " + String(this->locationHistory[0]) + ", " + String(this->locationHistory[1]) + ", " + String(this->locationHistory[2]));
    // Serial.println("vel = " + String(this->velocity));
    // Serial.println();
}

void Player::update(){

    bool res = this->updateMovement();

    if (res != 0){
        if (DEBUG) Serial.println("Player " + String(this->id) + " updateMovement() returned with id " + String(res));
    }

    // update hitbox
    this->hitbox.x = this->pos.x;
    this->hitbox.y = this->pos.y;

    // calculate velocity
    calculateVelocity();
    // Serial.println("Velocity = " + String(this->velocity));
}

int Player::updateMovement(){

    bool localDebug1 = false;
    static bool firstRound = true;

    // get distance
    int dist = getDistance(this->sensor);

    // validation
    if (dist < this->minDistance || dist > this->maxDistance) { return 1; }

    // map distance
    map(dist, 0, 100, 0, 50);

    // invert (since [0,0] is topleft)
    float dist_inverted = SCREEN_HEIGHT - dist - this->paddleSize.y;

    // discard if too little movement
    if (abs(dist_inverted - this->locationHistory[0]) < this->jitter){ return 2; }

    // discard if too much movement
    if (abs(dist_inverted - this->locationHistory[0] > this->maxMoveDelta && (firstRound == false))){
        this->newPosCounter++;
        if (this->newPosCounter >= this->newPosCounterLimit){
            newPosCounter = 0;
            Serial.println(F("new position assumed"));
            // and continue
        }
        else{ return 3;  }
    }

    // out of bounds check
    if (dist_inverted < 0) dist_inverted = 0;
    if (dist_inverted > SCREEN_HEIGHT - this->paddleSize.y) dist_inverted = SCREEN_HEIGHT - this->paddleSize.y;
    
    // smooth and apply (smoothing almost unnoticeable)
    float rate = 10.0f; // controls how fast smoothing is
    anim_float_to_target(&this->pos.y, dist_inverted, delta_t, rate);
    if (localDebug1) Serial.println("Dt = " + String(delta_t) + "    pos.y = " + String(this->pos.y) + "    dist_inverted = " + String(dist_inverted) + "    dist = " + String(dist));
    if (localDebug1) Serial.println("history = " + String(this->locationHistory[0]) + " | " + String(this->locationHistory[1]) + " | "  + String(this->locationHistory[2]));
    if (localDebug1) Serial.println("");

    // update location history 
    // TODO: fix this
    this->locationHistory[5] = this->locationHistory[4];
    this->locationHistory[4] = this->locationHistory[3];
    this->locationHistory[3] = this->locationHistory[2];
    this->locationHistory[2] = this->locationHistory[1];
    this->locationHistory[1] = this->locationHistory[0];
    this->locationHistory[0] = dist_inverted;

    // Serial.println("dist = " + String(dist) + "   dist_inve = " + String(dist_inverted));

    firstRound = false;

    return 0;
}