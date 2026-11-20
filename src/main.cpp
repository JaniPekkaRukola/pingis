/* 
Pingis
5G00ET65-3014 Sulautetun järjestelmän ohjelmointi ja mikrokontrollerit
11.2026 (init 3.11.2026)
*/
#include <Arduino.h>
// #include <EEPROM.h>
#include <assert.h>
#include "Adafruit_GFX.h" //https://github.com/adafruit/Adafruit-GFX-Library/blob/master/src/Adafruit_GFX.cpp
#include <Adafruit_SSD1306.h>
#include <Wire.h> //i2c
#include <buzzer.h>
// #include <avr/wdt.h> // watchdog TODO: implement this


// settings
// -------------------------------------------------
bool printFps = false;
bool printSensor = false;



// Pins
// -------------------------------------------------
const int BUTTON_1_PIN = 2;     // PD2
const int BUTTON_2_PIN = 3;     // PD3
const uint8_t SENSOR_P1_PIN = A0;   // PC0
const uint8_t SENSOR_P2_PIN = A1;   // PC1


#include "struct.hh"

// display
#define SCREEN_ADDRESS 0x3C // display
#define SSD1306_NO_SPLASH true
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);






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

enum guiState { //TODO: move this to "struct.hh"?
    guiNil,     // not used
    guiMenu,    // mainmenu
    guiGame,    // game
    guiStats    // stats from eeprom
};
guiState GUI = guiMenu;






// forward decls
int getDistance(uint8_t sensor);


#include "game.hh"
// #include "player.hh"

Game game;


void setup(){
    Serial.begin(9600); // TODO: raise this?
    Buzzer::initialization();

    // TODO: change these to register
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);

    // DDRD = 0b00001100;
    // DDRC = 0b00000011;

    // attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressed, FALLING);


    // random
    // randomSeed(analogRead(A2));
    // const int rand = random(5);
    // if (DEBUG) Serial.println("Random number was: " + String(rand));


    // display (from "ssd1306_128x64_i2c" example)
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { //   SSD1306_EXTERNALVCC
        Serial.println(F("[ERROR @setup()]: SSD1306 allocation failed"));
        assert(0);  // or for(;;);
    }

    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);
    display.setCursor((SCREEN_WIDTH / 2) - 20, SCREEN_HEIGHT / 2);
    display.print("STARTING");

    display.display();

    display.clearDisplay(); // clear display buffer

    // TODO: display startup logo


    GUI = guiGame;
    // GUI = guiMenu;
    // GUI = guiStats;

    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);

    game.startGame();
}





void loop(){

    // delta t
    unsigned long current_time = millis();
	delta_t = current_time - prev_time;
	prev_time = current_time; 
    int fps = (delta_t > 0) ? (1000.0 / delta_t) : 0;
    if (printFps) Serial.println("Delta_t = " + String(delta_t) + " ms\t fps = " + String(fps));

    display.clearDisplay(); // clear display buffer

    switch(GUI){
        case (guiMenu):{
            // drawTest(fps);

            // display.drawRect(10, 10, 50, 50, WHITE);
            // display.display();

        } break;
        case (guiGame):{
            game.update();
            game.draw();
            game.drawUI();

            // drawTest(fps);

            display.display();
            

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

int getDistance(uint8_t sensor){    // TODO: move this to a header file

    int highest = -1;
    int lowest = 2000;
    long totalADC = 0;

    for (int i = 0; i < NUM_READINGS; i++) {
        int rawADC = analogRead(sensor);

        // TODO: replace analogRead with registers:
        // NOTE: below not working
        // uint8_t luku, mask, res; // TODO: translate "luku" to english
        // if (sensor == A0) mask = 0b00000001;
        // else if (sensor == A1) mask = 0b00000010;
        // else assert(0);

        // luku = PINC;
        // int rawADC = luku & mask;
        
        if (rawADC > highest) highest = rawADC;
        if (rawADC < lowest) lowest = rawADC;
        
        totalADC += rawADC;
    }

    long filteredTotal = totalADC - highest - lowest;
    float averageADC = filteredTotal / 3.0; 
    float distance_cm = (averageADC / ADC_STEPS) * 520.0;

    distance_cm = (int) distance_cm;

    if (printSensor) {
        Serial.print(F("sensor("));
        Serial.print(sensor);
        Serial.println(") = " + String(distance_cm) + " cm");
    }

    return distance_cm;
}


int getId(){   // TODO: move this to a header file
    return random();
}





//TODO:
/*
-   switch to MEGA
-   sensor smoothing (paddle should not jump around if sensor reads bad values)
-   sensor max value (sensor reading should be disregarded if the value is > max accepted value) -> this prevents bad sensor readings (eg: bounceback from roof)
-   design UI
-   UI class?
-   eeprom save/read
-   score uart
-   watchdog?
-   timer
-   research if displays top yellow bar can be made blue
-   ball friction
-   player paddle velocity calculation
-   read sensor with registers
-   hook up buzzer logic
-   bugfix: ball should bounce back from the closest edge of the paddle (currently (atleast for p1) ball bounces back from the back edge when colliding)

*/