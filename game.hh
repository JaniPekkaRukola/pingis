#ifndef GAME_HH
#define GAME_HH

#include "player.hh"

typedef struct Vector2 {
    float x;
    float y;
};

// ball directions
typedef enum direction{
    dir_nil,
    dir_left,
    dir_right
};

typedef struct Ball {
    direction dir = dir_nil;

    Vector2 pos;
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

};

#endif