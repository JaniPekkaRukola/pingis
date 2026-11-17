#pragma once

// #include <iostream>
#include "struct.hh"
// #include <vector> // TODO: investigate if this could be replaced with velocitySampleCount sized vector instead of a whole lib

class Player{

    private:
        int id;     // for saving data to eeprom
        int score;  // move this to game obj?
        Vector2 pos;
        Hitbox hitbox;
        // std::vector<float> locationHistory; // players previous locations
        Vector2 velocity;
        static const int velocitySampleCount = 3; // how many prev positions are used for velocity calculations
        float locationHistory[velocitySampleCount] = {0.0f};

    public:
        Player();
        ~Player();

        void setPosX(int x);
        void setPosY(int y);
        int getPosX();
        int getPosY();
        bool setId(int id);
        Hitbox getHitbox();
        bool update();
        bool draw();

        
        void calculateVelocity();
        
        // TODO: fun methods like "decreaseHeight() -> makes paddle shorter" -> make these easier by doing stuff like "setPaddleSize" or "setPaddleWidth"
};