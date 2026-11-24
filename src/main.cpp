/* 
Pingis
5G00ET65-3014 Sulautetun järjestelmän ohjelmointi ja mikrokontrollerit
11.2026 (init 3.11.2026)
*/
#include <Arduino.h>
#include <EEPROM.h>
#include "Adafruit_GFX.h" //https://github.com/adafruit/Adafruit-GFX-Library/blob/master/src/Adafruit_GFX.cpp
#include <Adafruit_SSD1306.h>
#include <Wire.h> //i2c
#include "buzzer.hh"
#include "logo.hh"
// #include <avr/wdt.h> // watchdog TODO: implement this?


// hidden settings
// -------------------------------------------------
bool printFps = false;
bool printSensor = false;
bool enableMenuBall = true;



// Pins
// -------------------------------------------------
const uint8_t BUTTON_1 = (1 << PE4); // D2
const uint8_t BUTTON_2 = (1 << PE5); // D3


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
volatile bool button1Pressed = false;
volatile unsigned long button1PressTime = 0;
volatile bool button2Pressed = false;
volatile unsigned long button2PressTime = 0;
const unsigned long DEBOUNCE_DELAY = 20;

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
int menuIndex = 1;
int settingsIndex = 1;

// EEPROM
const uint16_t SIGNATURE = 0xAB42;
const int SIGNATURE_ADDR = 0;
const int DATA_ADDR = 2;




// forward decls
void initADC();
uint16_t readAnalog(uint8_t channel);
int getDistance(uint8_t sensor);
void loadEEPROM();
void drawMenu();
void drawSettings();
void drawMenuBall();
void drawTimeUP();
bool buttonPressCheck(uint8_t button, volatile bool &buttonPressed, volatile unsigned long &buttonPressTime);

#include "game.hh"

Game game;

void setup(){
    Serial.begin(115200);
    Buzzer::initialization();    
    game.gameTimerInitialization();
    initADC();
    randomSeed(readAnalog(2)); // seed for random ball direction on game start and on main menu
    loadEEPROM();

    DDRE &= ~(BUTTON_1 | BUTTON_2); // D2 and D3 -> INPUT
    PORTE |= BUTTON_1 | BUTTON_2; // D2 and D3 -> PULLUP ON
    // Trigger INT4 and INT5 on FALLING edges (button pressed)
    EICRB = 0b00001010;
    EIFR |= (1 << INTF4) | (1 << INTF5);
    EIMSK |= (1 << INT4) | (1 << INT5);

    // display (from "ssd1306_128x64_i2c" example)
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { //   SSD1306_EXTERNALVCC
        Serial.println(F("[ERROR @setup()]: SSD1306 allocation failed"));
        for(;;);
    }

    display.clearDisplay(); // clear display buffer

    display.cp437(true);
    display.setTextSize(2);
    display.drawBitmap(0, 0, logo_bmp, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE); // draw bitmap logo
    display.setTextColor(SSD1306_BLACK); // set color
    display.setCursor(1,1);
    display.print("Ykk\x94sryhm\x84"); // display name
    display.display();
    delay(1000);

    // reset display settings to default
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.clearDisplay(); // clear display buffer

    GUI = guiMenu;

    // game.startGame();
}

void loadEEPROM() {
    uint16_t storedSignature;
    EEPROM.get(SIGNATURE_ADDR, storedSignature);
    
    if (storedSignature != SIGNATURE) {
        EEPROM.put(DATA_ADDR, game.settings);
        EEPROM.put(SIGNATURE_ADDR, SIGNATURE);
    } else {
        EEPROM.get(DATA_ADDR, game.settings);
    }
};

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
        for(;;);
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

    // input
    if (buttonPressCheck(BUTTON_1, button1Pressed, button1PressTime)){
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
                settingsIndex++;
                if (settingsIndex > 4) settingsIndex = 1;
            } break;

            case (guiStats):{
                GUI = guiMenu;
            } break;

            default: { Serial.println(F("gui switch defaulted @button1Pressed")); }
        }



        /*
        if (GUI != guiMenu){

        // update gui
        GUI = static_cast<guiState>(static_cast<int>(GUI) + 1);
            
        if (GUI >= guiMax) GUI = guiMenu;
        Serial.print(F("GUI = "));
         Serial.println(GUI);
        } else {
        // multipurposing button 1 for menu selection and gui switching
        menuIndex++;
        if (menuIndex > 3) menuIndex = 1;
        }
        */
    }

    if (buttonPressCheck(BUTTON_2, button2Pressed, button2PressTime)){
        Serial.println("Button 2 was pressed");
        if (GUI == guiMenu){
            switch(menuIndex) {
                case 1:{ GUI = guiGame; game.startGame(); } break;
                case 2:{ GUI = guiSettings; } break;
                case 3:{ GUI = guiStats; } break;
                default:{ Serial.println(F("menuIndex switch defaulted")); }
            }
        } else if (GUI == guiSettings) {
            switch(settingsIndex) {
                case 1: {
                    game.settings.ballSpeed += 0.5f;
                    if (game.settings.ballSpeed > 4.0f) game.settings.ballSpeed = 1.0f;
                }
                break;

                case 2: {
                    game.settings.increaseSpeed = !game.settings.increaseSpeed;
                }
                break;

                case 3: {
                    game.settings.enableAudio = !game.settings.enableAudio;
                    if (game.settings.enableAudio == 1) Buzzer::play(Buzzer::SETTING);
                }
                break;

                case 4: {
                    EEPROM.put(DATA_ADDR, game.settings);
                    GUI = guiMenu;
                }
                break;

                default:{ Serial.println(F("settingsIndex switch defaulted")); }
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

            if (game.isTimeUp()) {
                game.stopGame();
                drawTimeUP();
                display.display();

                // Wait 5 seconds
                unsigned long timeNow = millis();
                while (millis() - timeNow < 5000);

                GUI = guiMenu;
                break;
            }
            game.update();
            game.draw();
            game.drawUI();

            // drawTest(fps);
            display.display();
        } break;

        case (guiSettings):{
            drawSettings();
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

void initADC() {
    DDRF &= ~((1 << PF0) | (1 << PF1)); // A0 and A1 -> INPUT
    DIDR0 |= (1 << ADC1D) | (1 << ADC0D);
    ADMUX = (1 << REFS0); // Reference voltage AVcc
    ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN); // 128 prescaler and enable ADC
}

uint16_t readAnalog(uint8_t channel) {
    ADMUX = (ADMUX & ~0b00011111) | (channel & 0x07); // select channel
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

int getDistance(uint8_t sensor){    // TODO: move this to a header file
    uint8_t channel = (sensor == A0) ? 0 : 1;

    int highest = -1;
    int lowest = 2000;
    long totalADC = 0;

    for (int i = 0; i < NUM_READINGS; i++) {
        int rawADC = readAnalog(channel);
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
        Serial.print(channel);
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
    float rx = random(0,100)/100.0;
    float ry = random(0,100)/100.0;
    float x = random(0,2);
    float y = random(0,2);
    x == 1 ? vel.x += rx : vel.x -= rx;
    y == 1 ? vel.y += ry : vel.y -= ry;
    vel.x = constrain(vel.x, -4.0f, 4.0f);
    vel.y = constrain(vel.y, -4.0f, 4.0f);
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

void drawSettings(){
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Settings");

    Vector2 boxSize = {8, 8};
    int padding = 4;
    int y_pos = 16;
    int x_pos = boxSize.x + padding;
    display.setTextSize(1);

    // increase ball speed
    (settingsIndex == 1) ?
        display.fillRect(0, y_pos, boxSize.x, boxSize.y, WHITE) :
        display.drawRect(0, y_pos, boxSize.x, boxSize.y, WHITE);
    display.setCursor(x_pos, y_pos);
    display.print("Ball speed: ");
    display.print(game.settings.ballSpeed);

    // ball's speed change after every round
    y_pos += boxSize.y + padding;
    (settingsIndex == 2) ?
        display.fillRect(0, y_pos, boxSize.x, boxSize.y, WHITE) :
        display.drawRect(0, y_pos, boxSize.x, boxSize.y, WHITE);
    display.setCursor(x_pos, y_pos);
    display.print("Ball speed up: ");
    display.print(game.settings.increaseSpeed ? "ON" : "OFF");

    // audio
    y_pos += boxSize.y + padding;
    (settingsIndex == 3) ?
        display.fillRect(0, y_pos, boxSize.x, boxSize.y, WHITE) :
        display.drawRect(0, y_pos, boxSize.x, boxSize.y, WHITE);
    display.setCursor(x_pos, y_pos);
    display.print("Audio: ");
    display.print(game.settings.enableAudio ? "ON" : "OFF");

    // back
    y_pos += boxSize.y + padding;
    (settingsIndex == 4) ?
        display.fillRect(0, y_pos, boxSize.x, boxSize.y, WHITE) :
        display.drawRect(0, y_pos, boxSize.x, boxSize.y, WHITE);
    display.setCursor(x_pos, y_pos);
    display.print("Back");
}

void drawTimeUP(){
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(16, 24);
    display.print("TIME UP!");
}

// D2 pin ISR
ISR(INT4_vect) {
    button1Pressed = true;
    button1PressTime = millis();
}

// D3 pin ISR
ISR(INT5_vect) {
    button2Pressed = true;
    button2PressTime = millis();
}

bool buttonPressCheck(uint8_t button, volatile bool &buttonPressed, volatile unsigned long &buttonPressTime) {
    if (!buttonPressed) return false;

    uint8_t oldSREG = SREG;
    cli();
    unsigned long pressedTime = buttonPressTime;
    SREG = oldSREG;

    if (millis() - pressedTime >= DEBOUNCE_DELAY) {
        buttonPressed = false;
        return ((PINE & (button)) == 0);
    }

    return false;
}


//TODO:
/*
-   sensor smoothing (paddle should not jump around if sensor reads bad values)
-   watchdog? <- is not strictly needed
-   score uart
-   ball friction
-   player paddle velocity calculation
-   bugfix: ball should bounce back from the closest edge of the paddle (currently (atleast for p1) ball bounces back from the back edge when colliding)
-   UI: add settings page
-   UI: add stats page (?)
*/