/* 
Pingis
5G00ET65-3014 Sulautetun järjestelmän ohjelmointi ja mikrokontrollerit
11.2026 (init 3.11.2026)
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <assert.h>
#include "Adafruit_GFX.h" //https://github.com/adafruit/Adafruit-GFX-Library/blob/master/src/Adafruit_GFX.cpp
#include <Adafruit_SSD1306.h>
#include <Wire.h> //i2c
#include <avr/wdt.h> // watchdog TODO: implement this

#include "struct.hh"

// display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// forward decls
void drawTest(int fps);
int getDistance(int sensor);



#include "game.hh"
// #include "player.hh"



void setup(){
    Serial.begin(9600); // TODO: raise this?

    // TODO: change these to register
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);


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
            drawTest(fps);

        } break;
        case (guiGame):{

        } break;
        case (guiStats):{

        } break;

        default: { assert(0); }
    }



    delay(500);
}

void drawTest(int fps){
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