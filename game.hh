#ifndef GAME_HH
#define GAME_HH

#include <tuple>
#include "player.hh"

// ball directions
typedef enum direction{
    dir_nil,
    dir_left_top,
    dir_left_mid,
    dir_left_low,
    dir_right_top,
    dir_right_mid,
    dir_right_low
};

class Game{

    private:
        Player p1;
        Player p2;
        int score;
        bool gameActive;
        int gameID;
        std::tuple<int, int> ballPos; // testing tuple for pos
        direction ballDirection = dir_nil;

    private:
        Game();
        ~Game();

        bool setScore(int s);
        int getScore();
        bool update();

};

#endif