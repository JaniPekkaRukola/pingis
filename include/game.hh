#pragma once

#include "player.hh"
#include "ball.hh"
#include "struct.hh"

class Game{
    private:
        Player p1;
        Player p2;
        Ball ball;

        // int gameID; // useless?
        int playerOffset = 5; // how much players are offset from the border

        int p1_score;
        int p2_score;

        int winScore = 25;  // game win score

        bool settingsLoaded = false;    // have settings been loaded succesfully from eeprom?
        bool statsLoaded = false;       // have stats    been loaded succesfully from eeprom?
    
    public:

        struct gameSettings {
            float ballSpeed = 1.0f;     // yeah
            bool increaseSpeed = true;  // ball increases speed with every goal
            bool enableAudio = false;   // enables buzzer
            bool drawScore = false;     // draws score to the screen
        };

        struct gameStats {
            int gamesPlayed;
            int totalGoals;
            long distanceReadTimes;
        };

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
        gameSettings settings;
        gameStats stats = {0};
        bool askToResetStats = false;
        // bool resetGame();
        
        bool settingsLoaded = false;
        bool statsLoaded = false;
        void sendScoreUART(const char* event);
};