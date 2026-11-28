#pragma once

// #include <iostream>
#include "struct.hh"
#include <stdint.h>

class Player{

    private:
        int id;
        Vector2 pos;
        Hitbox hitbox;
        float velocity;                     // y-velocity
        float maxMoveDelta = 20.0f;         // max accepted move-amount in a singe 'frame'. in cm
        float maxDistance = SCREEN_HEIGHT;  // max accepted distance for paddle in cm
        float minDistance = 1.0f;           // min accepted distance for paddle in cm
        float jitter = 3.0f;                // how many cm can paddle shake without movement acknowledgment
        int newPosCounter = 0;              // if paddle moves to a new position (without the travel eg: teleport), this counter tracks looping errors
        const int newPosCounterLimit = 5;   // when "newPosCounter" hits this limit, paddle's new position is assumed

        // TODO: sample count should be higher prolly
        // TODO: this means locationHistory array append must be done manually
        static const int velocitySampleCount = 6; // how many prev positions are used for velocity calculations
        float locationHistory[velocitySampleCount] = {0.0f};

    public:
        Player();
        ~Player();

        void setPosX(int x);
        void setPosY(int y);
        int getPosX();
        int getPosY();
        void setId(int id);
        Hitbox getHitbox();
        bool setWidth(int w);
        bool setHeight(int h);
        void calculateVelocity();
        float getVelocity();
        // bool acceptMovement(int dist);
        void update();
        int updateMovement();
        void reset();

        // bool update_old();
        bool draw();
        
        uint8_t sensor;
        Vector2 paddleSize = {2, 15};
        
        // TODO: fun methods like "decreaseHeight() -> makes paddle shorter" -> make these easier by doing stuff like "setPaddleSize" or "setPaddleWidth"
};