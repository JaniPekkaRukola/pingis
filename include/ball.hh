#pragma once

#include "struct.hh"

class Ball{
    private:
        Dir dir;

        // Vector2 pos;
        Hitbox hitbox;
        Vector2 velocity;
        float friction; // how much velocity ball inherits from paddle hit (paddle_vel * friction)


    public:

        Vector2 getPos();
        bool update();
        bool draw();
        Dir getDir();
};