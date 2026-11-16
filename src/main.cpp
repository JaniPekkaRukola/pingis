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
// #include <Wire.h> //i2c

// display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3D
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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

// delta t
unsigned long prev_time = 0;
unsigned long delta_t = 0;

enum guiState {
    guiNil,     // not used
    guiMenu,    // mainmenu
    guiGame,    // game
    guiStats    // stats from eeprom
};
guiState GUI = guiMenu;

// enum gameMode{} ? like: normal, increasing speed, gravity? (ball gravitates lower), ball is invisible sometimes etc

#include "game.hh"
// #include "player.hh"



void setup(){
    Serial.begin(9600); // TODO: raise this?

    // pinMode(BUTTON_PIN, INPUT_PULLUP);


    // attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressed, FALLING);


    // random
    // randomSeed(analogRead(A2));
    // const int rand = random(5);
    // if (DEBUG) Serial.println("Random number was: " + String(rand));


    // display (from "ssd1306_128x64_i2c" example)
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("[ERROR @setup()]: SSD1306 allocation failed"));
        assert(0);  // or for(;;);
    }

    display.display();
    delay(2000); // TODO: Test is necessary

    display.clearDisplay(); // clear display buffer

    // TODO: display startup logo

}

void loop(){

    // delta t
    unsigned long current_time = millis();
	delta_t = current_time - prev_time;
	prev_time = current_time; 
    int fps = (delta_t > 0) ? (1000.0 / delta_t) : 0;
    if (DEBUG) Serial.println("Delta_t = " + String(delta_t) + " ms\t fps = " + String(fps));
    // hopefully delta_t is more than 30

    display.clearDisplay(); // clear display buffer

    switch(GUI){
        case (guiMenu):{
            drawTest();

        } break;
        case (guiGame):{

        } break;
        case (guiStats):{

        } break;

        default: { assert(0); }
    }



    delay(500);
}

void drawTest(){
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("DrawTest"));
    display.setCursor(2, 0);
    display.println("Delta_t = " + String(delta_t) + " ms");
    display.setCursor(2, 0);
    display.println("fps = " + String(fps));

    display.display();

    if(DEBUG) Serial.println(F("drawtest..."));
}

/*
void buttonPressed(){
  static unsigned long lastPressTime = 0;
 
  if (millis() - lastPressTime >= debounceDelay){
    wasButtonPressed = true;
    lastPressTime = millis();
  }
}*/


int getDistance(int sensor){    // TODO: move this to a header file
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

    long filteredTotal = totalADC - highest - lowest;
    float averageADC = filteredTotal / 3.0; 
    float distance_cm = (averageADC / ADC_STEPS) * 520.0;

    return distance_cm;
}

int getId(){   // TODO: move this to a header file
    return random();
}