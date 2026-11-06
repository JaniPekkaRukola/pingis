#ifndef GAME_HH
#define GAME_HH

#include "player.hh"
#include "struct.hh"


typedef struct Ball {

    // ngl this kinda shit
    bool dir_up = true;     // true = up,  false = down
    bool dir_right = true;  // true = right,  false = left

    // Vector2 pos;
    Hitbox hitbox;
    Vector2 velocity;
    float friction = 0.1f; // how much velocity ball inherits from paddle hit (paddle_vel * friction)

};

class Game{

    private:
        Player p1;
        Player p2;
        Ball ball = {};
        int score;
        bool gameActive;
        int gameID;


    private:
        Game();
        ~Game();

        bool setScore(int s);
        int getScore();
        bool update();
        bool updateBall();
        bool checkCollision(Player p);
        Vector2 getBallPos();


};

#endif