#pragma once
#include <Arduino.h> // for fabs and pow

// Defines
// -------------------------------------------------

#define DEBUG false
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

inline bool anim_float_to_target(float* value, float target, float delta_t, float rate){
    // "animates" value towards target by rate
    *value += (target - *value) * (1.0 - pow(2.0f, -rate * delta_t));
    if (fabs(*value - target) <= 0.001f){
        *value = target;
        return true;    // reached
    }
    return false;
}

/*
void appendArr(float arr[], float val){
    // TODO: this
    assert(!arr[0]);

    int size = sizeof(arr) / sizeof(arr[0]);

    // arr[3] = arr[2];
    // arr[2] = arr[1];
    // arr[1] = val;

}*/