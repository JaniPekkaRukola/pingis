#pragma once

// #include <iostream>
#include "struct.hh"

class Player{

    private:
        int id;     // for saving data to eeprom
        int score;  // move this to game obj?
        // int posX;   // posX shouldn't change after init
        // int posY;
        // int width;  // paddle width in pixels
        Hitbox hitbox;

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

};