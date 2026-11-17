#pragma once

#include <Arduino.h>
#include <assert.h>


// Defines
// -------------------------------------------------

#define DEBUG true
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define WHITE SSD1306_WHITE

// drawrect macro
#define drawRectHB(display, hitbox, c) display.drawRect((hitbox).x, (hitbox).y, (hitbox).width, (hitbox).height, (c))







// Structs
// -------------------------------------------------

struct Dir { // TODO: move this back to "ball.hh" cause not needed elsewhere?
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