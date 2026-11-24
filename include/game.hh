#pragma once

#include "player.hh"
#include "ball.hh"
#include "struct.hh"

class Game{

    private:
        Player p1;
        Player p2;
        Ball ball;

        Vector2 paddleSize = {2, 20};
        int gameID;
        int playerOffset; // how much players are offset from the border

        int p1_score;
        int p2_score;

        bool drawScore;     // draws score to the screen
        int winScore = 25;  // game win score
        
        
        public:
        Game();
        ~Game();
        
        bool gameActive;
        bool update();
        void draw();
        void drawUI();
        bool checkCollision(Player p);
        void startGame();
        void stopGame();
        void roundReset();

        void gameTimerInitialization();
        void startGameTimer();
        bool isTimeUp();
        GameSettings settings;
        // bool resetGame();
};