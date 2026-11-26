#pragma once

// Defines
// -------------------------------------------------

#define DEBUG true
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define WHITE SSD1306_WHITE
#define ALLOWSTATSRESET true // can stats be reset from eeprom during an error. This should be false if stats are actually important


// drawrect macro
#define drawRectHB(display, hitbox, c) display.drawRect((hitbox).x, (hitbox).y, (hitbox).width, (hitbox).height, (c))




// Structs
// -------------------------------------------------

struct Dir {
    bool right = false;
    bool up = false;
    bool stationary = true;
};

struct Vector2 {
    float x;
    float y;
};

struct Hitbox {
    float x;
    float y;
    float width;
    float height;
};


const Vector2 charSize_1 = {6, 8};      // display's character size in pixels (when text size 1);
const Vector2 charSize_2 = {12, 16};    // display's character size in pixels (when text size 2);

// enum gameMode{} ? like: normal, increasing speed, gravity? (ball gravitates lower), ball is invisible sometimes etc

// Functions
// -------------------------------------------------

/*
void appendArr(float arr[], float val){
    // TODO: this
    assert(!arr[0]);

    int size = sizeof(arr) / sizeof(arr[0]);

    // arr[3] = arr[2];
    // arr[2] = arr[1];
    // arr[1] = val;

}*/