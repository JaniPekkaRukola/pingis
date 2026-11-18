#pragma once

#include "struct.hh"

class Ball{
    private:
        Dir dir;

        Vector2 pos;
        Hitbox hitbox;
        Vector2 velocity;
        float friction; // how much velocity ball inherits from paddle hit (paddle_vel * friction)
        void updateHitbox();


    public:

        Ball();
        ~Ball();
        Vector2 getPos();
        bool update();
        bool draw();
        Dir getDir();
        bool setDir(bool right, bool up, bool stationary);
        bool setSize(int w, int h);
        Vector2 getSize();
        bool setPos(float x, float y);

        Vector2 getVelocity();
        bool setVelocity(Vector2 v);

        // TODO: fun methods like "increaseVelocity" or "decreaseSize" or make the ball's color black for a second (invisible)
};