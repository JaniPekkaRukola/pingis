#pragma once

#include "player.hh"
#include "ball.hh"
#include "struct.hh"

class Game{

    private:
        Player p1;
        Player p2;
        Ball ball;
        int score;
        bool gameActive;
        int gameID;


    private:
        Game();
        ~Game();

        bool setScore(int s);
        int getScore();
        bool update();
        bool draw();
        bool checkCollision(Player p);

};