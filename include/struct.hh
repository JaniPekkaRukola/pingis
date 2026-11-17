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

#define SCREEN_ADDRESS 0x3D // display



// Pins
// -------------------------------------------------
const int BUTTON_1_PIN = 2;     // PD2
const int BUTTON_2_PIN = 3;     // PD3
const int SENSOR_P1_PIN = A0;   // PC0
const int SENSOR_P2_PIN = A1;   // PC1



// variables
// -------------------------------------------------

// ADC
const float V_REF = 5.0;        // Analog reference voltage ( 5V )
const float R_BITS = 10.0;      // ADC resolution (bits)
const float ADC_STEPS = (1 << int(R_BITS)) - 1; // Number of steps (2^R_BITS - 1)

// Filter VARS
const int NUM_READINGS = 5;

// button vars
bool wasButtonPressed = false;
unsigned int buttonPressCount = 0;
unsigned long debounceDelay = 150;

// delta t
unsigned long prev_time = 0;
unsigned long delta_t = 0;



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

enum guiState {
    guiNil,     // not used
    guiMenu,    // mainmenu
    guiGame,    // game
    guiStats    // stats from eeprom
};
guiState GUI = guiMenu;

// enum gameMode{} ? like: normal, increasing speed, gravity? (ball gravitates lower), ball is invisible sometimes etc

// Functions
// -------------------------------------------------

void appendArr(float arr[], float val){
    // TODO: this
    assert(!arr[0]);

    int size = sizeof(arr) / sizeof(arr[0]);

    // arr[3] = arr[2];
    // arr[2] = arr[1];
    // arr[1] = val;

}