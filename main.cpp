/* 
Pingis
5G00ET65-3014 Sulautetun järjestelmän ohjelmointi ja mikrokontrollerit
11.2026 (init 3.11.2026)
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <assert.h>
// #include "Adafruit_GFX.h" //https://github.com/adafruit/Adafruit-GFX-Library/blob/master/src/Adafruit_GFX.cpp
// #include <Adafruit_SSD1306.h>

#include "game.hh"
// #include "player.hpp"


// pins
// const int BUTTON_PIN = ;
const int ADC_PIN = A0;
const int SENSOR = A1;

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

typedef enum guiState {
    guiNil,     // not used
    guiMenu,    // mainmenu
    guiGame,    // game
    guiStats    // stats from eeprom
};
guiState GUI = guiMenu;



void setup(){
    // Serial.begin(9600);

    // pinMode(BUTTON_PIN, INPUT_PULLUP);


    // attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressed, FALLING);


    // random
    // randomSeed(analogRead(A2));
    // const int rand = random(5);
    // if (DEBUG) Serial.println("Random number was: " + String(rand));
}

void loop(){

    switch(GUI){
        case (guiMenu):{

        } break;
        case (guiGame):{

        } break;
        case (guiStats):{

        } break;

        default: { assert(0); }
    }

}


/*
void buttonPressed(){
  static unsigned long lastPressTime = 0;
 
  if (millis() - lastPressTime >= debounceDelay){
    wasButtonPressed = true;
    lastPressTime = millis();
  }
}*/

int getDistance(int sensor){
    int highest = -1;
    int lowest = 2000;
    long totalADC = 0;
    int delayAmount = 20; // study this

    for (int i = 0; i < NUM_READINGS; i++) {
        int rawADC = analogRead(sensor);
        
        if (rawADC > highest) highest = rawADC;
        if (rawADC < lowest) lowest = rawADC;
        
        totalADC += rawADC;
        delay(delayAmount);
    }

    Long filteredTotal = totalADC - highest - lowest;
    float averageADC = filteredTotal / 3.0; 
    float distance_cm = (averageADC / ADC_STEPS) * 520.0;

    return distance_cm;
}

int getId(){
    return random();
}