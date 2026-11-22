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
#include "logo.hh"
// #include <avr/wdt.h> // watchdog TODO: implement this


// settings
// -------------------------------------------------
bool printFps = false;
bool printSensor = false;
bool enableMenuBall = true;



// Pins
// -------------------------------------------------
const uint8_t BUTTON_1_PIN = 3;     // PD3 MEGA: PE5
const uint8_t BUTTON_2_PIN = 2;     // PD2 MEGA: PE4
const uint8_t SENSOR_P1_PIN = A0;   // PC0 MEGA: PF0
const uint8_t SENSOR_P2_PIN = A1;   // PC1 MEGA: PF1


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

// Button VARS
// volatile bool wasButton1Pressed = false; // OLD button
// volatile bool wasButton2Pressed = false; // OLD button
volatile bool button1Pressed = false; // V2 button
volatile unsigned long button1PressTime = 0; // V2 button
volatile bool button2Pressed = false; // V2 button
volatile unsigned long button2PressTime = 0; // V2 button
const unsigned long DEBOUNCE_DELAY = 20; // OLD button version was 250 <- In V2 version do NOT make this big, maybe test like 30ms if 20ms is bad?

// delta t
unsigned long prev_time = 0;
unsigned long delta_t = 0;

enum guiState { //TODO: move this to "struct.hh"?
    guiNil = 0,     // not used
    guiMenu,    // mainmenu
    guiGame,    // game
    guiSettings,
    guiStats,    // stats from eeprom
    guiMax
};
guiState GUI = guiMenu;
unsigned int menuIndex = 1;





// forward decls
int getDistance(uint8_t sensor);
void drawMenu();
void drawMenuBall();
void button1_time(); // V2 button for ISR
void button2_time(); // V2 button for ISR
bool buttonPressCheck(uint8_t pin, volatile bool &buttonPressed, volatile unsigned long &buttonPressTime); // V2 button
// void button_1_Pressed(); // OLD button
// void button_2_Pressed(); // OLD button

#include "game.hh"

Game game;


void setup(){
    Serial.begin(9600); // TODO: raise this?
    Buzzer::initialization();

    // TODO: change these to register
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);

    // DDRD = 0b00001100;
    // DDRC = 0b00000011;

    attachInterrupt(digitalPinToInterrupt(BUTTON_1_PIN), button1_time, FALLING); // V2 button
    attachInterrupt(digitalPinToInterrupt(BUTTON_2_PIN), button2_time, FALLING); // V2 button
    //attachInterrupt(digitalPinToInterrupt(BUTTON_1_PIN), button_1_Pressed, FALLING); // OLD button
    //attachInterrupt(digitalPinToInterrupt(BUTTON_2_PIN), button_2_Pressed, FALLING); // OLD button


    // random
    // randomSeed(analogRead(A2));
    // const int rand = random(5);
    // if (DEBUG) Serial.println("Random number was: " + String(rand));


    // display (from "ssd1306_128x64_i2c" example)
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { //   SSD1306_EXTERNALVCC
        Serial.println(F("[ERROR @setup()]: SSD1306 allocation failed"));
        assert(0);  // or for(;;);
    }

    display.clearDisplay(); // clear display buffer

    display.setTextSize(2);
    display.drawBitmap(0, 0, logo_bmp, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE); // draw bitmap logo
    display.setTextColor(SSD1306_BLACK);            // set color
    display.setCursor(1,1);
    display.print("Ykkosryhma");                    // display name
    display.display();
    delay(1000);

    // reset display settings to default
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.clearDisplay(); // clear display buffer

    GUI = guiMenu;

    // game.startGame();
}


void bombo(){
    static int x = 0;
    static unsigned long last = 0;
    Vector2 end = {93,18};
    display.drawBitmap(0, 0, bombo_bmp, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
    display.drawLine(x, end.y, end.x, end.y, WHITE);
    delay(10);
    if (millis() - last >= (unsigned long)map(x, 0, end.x, 500, 20)){
        last = millis(); 
        Buzzer::play(Buzzer::PADDLE);
    }
    x++;
    if (x > end.x){
        display.clearDisplay();
        display.display();
        assert(0);
    }
}


void loop(){

    // delta t
    unsigned long current_time = millis();
	delta_t = current_time - prev_time;
	prev_time = current_time; 
    int fps = (delta_t > 0) ? (1000.0 / delta_t) : 0;
    if (printFps) Serial.println("Delta_t = " + String(delta_t) + " ms\t fps = " + String(fps));

    display.clearDisplay(); // clear display buffer

    /*
    // OLD button version started:
    if (wasButton1Pressed){
        wasButton1Pressed = false;

        static unsigned long lastPressTime = 0;
        if (millis() - lastPressTime >= DEBOUNCE_DELAY){
            wasButton1Pressed = true;
            lastPressTime = millis();
            Serial.print(F("Button 1 pressed "));
        }
        ...
    */
    // input
    if (buttonPressCheck(BUTTON_1_PIN, button1Pressed, button1PressTime)){
        Serial.println("Button 1 was pressed");
        switch(GUI){

            case (guiGame):{
                game.stopGame();
                GUI = guiMenu;
            } break;

            case (guiMenu):{
                menuIndex++;
                if (menuIndex > 3) menuIndex = 1;
            } break;

            case (guiSettings):{
                // TODO: save settings to eeprom?
                GUI = guiMenu;
            } break;

            case (guiStats):{
                GUI = guiMenu;
            } break;

            default: { Serial.println(F("gui switch defaulted @wasButton1Pressed")); }
        }



        // if (GUI != guiMenu){

        //     // update gui
        //     GUI = static_cast<guiState>(static_cast<int>(GUI) + 1);
            
        //     if (GUI >= guiMax) GUI = guiMenu;
        //     Serial.print(F("GUI = "));
        //     Serial.println(GUI);
            
        // }
        // else{
        //     // multipurposing button 1 for menu selection and gui switching
        //     menuIndex++;
        //     if (menuIndex > 3) menuIndex = 1;
        // }
        
    }

    /*
    OLD button version started:
    if (wasButton2Pressed){
        wasButton2Pressed = false;
        ....
    }
    */
    if (buttonPressCheck(BUTTON_2_PIN, button2Pressed, button2PressTime)){
        Serial.println("Button 2 was pressed");
        if (GUI == guiMenu){
            switch(menuIndex){
                case 1:{ GUI = guiGame; } break;
                case 2:{ GUI = guiSettings; } break;
                case 3:{ GUI = guiStats; } break;
                default:{ Serial.println(F("menuIndex switch defaulted")); }
            }
        }
    }

    switch(GUI){
        case (guiMenu):{
            drawMenu();
            if (enableMenuBall) drawMenuBall();
            display.display();

        } break;

        case (guiGame):{
            game.update();
            game.draw();
            game.drawUI();

            // drawTest(fps);

            display.display();
        } break;

        case (guiSettings):{
            display.setCursor(0, 0);
            display.setTextSize(2);
            display.print("Settings");

            bombo();

            display.display();
        } break;

        case (guiStats):{
            display.setCursor(0, 0);
            display.setTextSize(2);
            display.print("Stats");
            display.display();
        } break;

        default: { Serial.println(F("Main Gui switch defaulted")); GUI = guiMenu; menuIndex = 1; }
    }

}

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

void drawMenuBall(){
    // draws a bouncing ball over the menu
    static Vector2 pos = {0,0};
    static Vector2 vel = {1.1, 1.2};
    const int size = 2;
    if (pos.x + vel.x > SCREEN_WIDTH || pos.x + vel.x < 0) vel.x *= -1;
    if (pos.y + vel.y > SCREEN_HEIGHT || pos.y + vel.y < 0) vel.y *= -1;
    pos.x += vel.x;
    pos.y += vel.y;
    display.drawRect(pos.x, pos.y, size, size, WHITE);
    float rx = random(0,100)/100;
    float ry = random(0,100)/100;
    float x = random(0,1);
    float y = random(0,1);
    x == 1 ? vel.x += rx : vel.x -= rx;
    y == 1 ? vel.y += ry : vel.y -= ry;
}

void drawMenu(){
    display.setTextColor(SSD1306_WHITE);
    
    // Title (draw "P" normally first and the rest 1px higher)
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("P");
    display.setCursor(charSize_2.x + 1, -1);
    display.print("ingis");
    display.drawLine(0, 14, SCREEN_WIDTH, 14, WHITE);

    // settings
    Vector2 boxSize = {10, 10};
    int padding = 5;
    int y_pos = 16 + 3;
    int x_pos = boxSize.x + padding;
    display.setTextSize(1);

    // new game
    (menuIndex == 1) ? 
        display.fillRect(0, y_pos, boxSize.x, boxSize.y, WHITE):
        display.drawRect(0, y_pos, boxSize.x, boxSize.y, WHITE); 
    display.setCursor(x_pos, y_pos + 1);
    display.print("New game");

    // settings
    y_pos += boxSize.y + padding;
    (menuIndex == 2) ? 
        display.fillRect(0, y_pos, boxSize.x, boxSize.y, WHITE):
        display.drawRect(0, y_pos, boxSize.x, boxSize.y, WHITE);
    display.setCursor(x_pos, y_pos + 1);
    display.print("Settings");

    // stats
    y_pos += boxSize.y + padding;
    (menuIndex == 3) ? 
        display.fillRect(0, y_pos, boxSize.x, boxSize.y, WHITE):
        display.drawRect(0, y_pos, boxSize.x, boxSize.y, WHITE);
    display.setCursor(x_pos, y_pos + 1);
    display.print("Stats");

    // draw bottom line
    display.drawLine(0, SCREEN_HEIGHT-1, SCREEN_WIDTH, SCREEN_HEIGHT-1, WHITE);
}

// V2 button ISR
void button1_time(){
    button1Pressed = true;
    button1PressTime = millis();
}

// V2 button ISR
void button2_time(){
    button2Pressed = true;
    button2PressTime = millis();
}

<<<<<<< HEAD

=======
// V2 button check if button is pressed
bool buttonPressCheck(uint8_t pin, volatile bool &buttonPressed, volatile unsigned long &buttonPressTime) {
    if (!buttonPressed) return false;

    uint8_t oldSREG = SREG;
    cli();
    unsigned long pressedTime = buttonPressTime;
    SREG = oldSREG;

    if (millis() - pressedTime >= DEBOUNCE_DELAY) {
        buttonPressed = false;
        return (digitalRead(pin) == LOW);
    }

    return false;
}

// OLD button
/*
>>>>>>> d449dfa23c8d7f5cc7bea4fa8b86074abd18b876
void button_1_Pressed(){
    static unsigned long lastPressTime = 0;
    if (millis() - lastPressTime >= DEBOUNCE_DELAY){
        wasButton1Pressed = true;
        lastPressTime = millis();
        Serial.print(F("Button 1 pressed "));
    }
}
*/

// OLD button
/*
void button_2_Pressed(){
    static unsigned long lastPressTime = 0;
    if (millis() - lastPressTime >= DEBOUNCE_DELAY){
        wasButton2Pressed = true;
        lastPressTime = millis();
        Serial.print(F("Button 2 pressed "));
    }
}
*/


//TODO:
/*
-   switch to MEGA <-- MEGA "version" is ready 20.11., uncomment every code line where is "MEGA" keyword (and remove the equivalent Nano code line)
-   sensor smoothing (paddle should not jump around if sensor reads bad values)
-   sensor max value (sensor reading should be disregarded if the value is > max accepted value) -> this prevents bad sensor readings (eg: bounceback from roof)
-   eeprom save/read
-   score uart
-   watchdog?
-   timer
-   ball friction
-   player paddle velocity calculation
-   read sensor with registers
-   bugfix: ball should bounce back from the closest edge of the paddle (currently (atleast for p1) ball bounces back from the back edge when colliding)
-   get rid of assert. dont really work with pio
-   button(s) activate even when releasing the button (isr acts like set to "CHANGE" but its set to "FALLING") investigate
-   start game by clicking a button
-   UI: add settings page
-   UI: add stats page (?)
-   fix logo (Ykkosryhma) 'ä' and 'ö'
*/