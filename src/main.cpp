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
    guiTimeUP,
    guiMax
};
guiState GUI = guiMenu;
unsigned long timeUPstart = 0;
int menuIndex = 1;
int settingsIndex = 1;


// forward decls
void initADC();
uint16_t readAnalog(uint8_t channel);
int getDistance(uint8_t sensor);
void loadSettingsEEPROM();
void loadStatsEEPROM();
void loadMatchEEPROM();
void saveMatchEEPROM(int p1Score, int p2Score); 
void drawMenu();
void drawSettings();
void drawStats();
void drawMenuBall();
void drawTimeUP();
void centerTextAndPrint(Adafruit_SSD1306 display, const Printable& text, int textWidth, Vector2 charsize);
bool buttonPressCheck(uint8_t button, volatile bool &buttonPressed, volatile unsigned long &buttonPressTime);

#include "game.hh"

Game game;

// EEPROM
// settings
const uint16_t SIGNATURE_settings = 0xABCD;
const int SIGNATURE_ADDR_settings = 0;
const int DATA_ADDR_settings = SIGNATURE_ADDR_settings + 2;

// stats
const uint16_t SIGNATURE_stats = 0xDCBA;
const int SIGNATURE_ADDR_stats = DATA_ADDR_settings + sizeof(Game::gameSettings);
const int DATA_ADDR_stats = SIGNATURE_ADDR_stats + 2;

struct LastMatch {
    int p1Score;
    int p2Score;
};
LastMatch lastMatch = {0, 0};

const uint16_t SIGNATURE_match = 0xCAFE;
const int SIGNATURE_ADDR_match = DATA_ADDR_stats + sizeof(Game::gameStats);
const int DATA_ADDR_match = SIGNATURE_ADDR_match + 2;

void setup(){
    Serial.begin(115200);
    Buzzer::initialization();    
    game.gameTimerInitialization();
    initADC();
    randomSeed(readAnalog(2)); // seed for random ball direction on game start and on main menu
    loadSettingsEEPROM();
    loadStatsEEPROM();
    loadMatchEEPROM();

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

    // Serial.println("\n\nEEPROM:");
    // Serial.println("SIGNATURE_settings = " + String(SIGNATURE_settings));
    // Serial.println("SIGNATURE_ADDR_settings = " + String(SIGNATURE_ADDR_settings));
    // Serial.println("DATA_ADDR_settings = " + String(DATA_ADDR_settings));
    // Serial.println("SIGNATURE_stats = " + String(SIGNATURE_stats));
    // Serial.println("SIGNATURE_ADDR_stats = " + String(SIGNATURE_ADDR_stats));
    // Serial.println("DATA_ADDR_stats = " + String(DATA_ADDR_stats));
}

void loadSettingsEEPROM() {
    uint16_t storedSIGNATURE_settings;
    EEPROM.get(SIGNATURE_ADDR_settings, storedSIGNATURE_settings);
    
    if (storedSIGNATURE_settings != SIGNATURE_settings) {
        EEPROM.put(DATA_ADDR_settings, game.settings);
        EEPROM.put(SIGNATURE_ADDR_settings, SIGNATURE_settings);
    } else {
        EEPROM.get(DATA_ADDR_settings, game.settings);
    }
}

bool checkStatsSignature(){
    uint16_t storedSIGNATURE_stats;
    EEPROM.get(SIGNATURE_ADDR_stats, storedSIGNATURE_stats);
    if (storedSIGNATURE_stats == SIGNATURE_stats) return true;
    return false;
}

void loadStatsEEPROM(){
    if (checkStatsSignature()){
        EEPROM.get(DATA_ADDR_stats, game.stats);
        // game.statsLoaded = true;
        Serial.println(F("Stats loaded succesfully from eeprom"));
        
        // sanity check
        if (game.stats.gamesPlayed < 0 || game.stats.gamesPlayed > 1000000) game.stats.gamesPlayed = 0;
        if (game.stats.totalGoals < 0 || game.stats.totalGoals > 1000000) game.stats.totalGoals = 0;
        // if (game.stats.playtime < 0 || game.stats.playtime > 100000000) game.stats.playtime = 0;
        if (game.stats.distanceReadTimes < 0 || game.stats.distanceReadTimes > 100000000) game.stats.distanceReadTimes = 0;
    }
    else{
        Serial.println(F("[ERROR: @loadStatsEEPROM()] Failed to load stats from eeprom (signature mismatch). Stats = 0, eeprom not affected"));
        game.stats = {0};
    }
}

void saveStatsEEPROM(){
    if (checkStatsSignature()){
        EEPROM.put(DATA_ADDR_stats, game.stats);
        EEPROM.put(SIGNATURE_ADDR_stats, SIGNATURE_stats);
        Serial.println(F("stats saved to eeprom succesfully"));
    }
    else{
        if (ALLOWSTATSRESET){
            Game::gameStats stats_temp = {0};
            EEPROM.put(DATA_ADDR_stats, stats_temp);
            EEPROM.put(SIGNATURE_ADDR_stats, SIGNATURE_stats);
            Serial.println(F("stats reset to 0 and saved to eeprom succesfully"));
        }
        else{
            Serial.println(F("[ERROR: @loadStatsEEPROM()] Failed to load stats from eeprom (signature mismatch).. Also 'ALLOWSTATSRESET' is disabled so if the stats are important, dump them and figure it out"));
        }
    }
}

void loadMatchEEPROM(){
    uint16_t storedSIGNATURE_match;
    EEPROM.get(SIGNATURE_ADDR_match, storedSIGNATURE_match);

    if (storedSIGNATURE_match == SIGNATURE_match){
        EEPROM.get(DATA_ADDR_match, lastMatch);
    }
    else{
        lastMatch = {0, 0};
        EEPROM.put(DATA_ADDR_match, lastMatch);
        EEPROM.put(SIGNATURE_ADDR_match, SIGNATURE_match);
    }
}

void saveMatchEEPROM(int p1Score, int p2Score){
    lastMatch.p1Score = p1Score;
    lastMatch.p2Score = p2Score;
    EEPROM.put(DATA_ADDR_match, lastMatch);
    EEPROM.put(SIGNATURE_ADDR_match, SIGNATURE_match);
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

    // BUTTON 1
    if (buttonPressCheck(BUTTON_1, button1Pressed, button1PressTime)){
        if (DEBUG) Serial.println(F("Button 1 was pressed"));
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
                game.askToResetStats = false;
            } break;

            default: { Serial.println(F("gui switch defaulted @button1Pressed")); }
        }
    }

    // BUTTON 2
    if (buttonPressCheck(BUTTON_2, button2Pressed, button2PressTime)){
        if (DEBUG) Serial.println(F("Button 2 was pressed"));

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
                } break;

                case 2: {
                    game.settings.increaseSpeed = !game.settings.increaseSpeed;
                } break;

                case 3: {
                    game.settings.enableAudio = !game.settings.enableAudio;
                    if (game.settings.enableAudio == 1) Buzzer::play(Buzzer::SETTING);
                } break;

                case 4: {
                    EEPROM.put(DATA_ADDR_settings, game.settings);
                    settingsIndex = 1;
                    GUI = guiMenu;
                } break;

                default:{ Serial.println(F("settingsIndex switch defaulted")); }
            }
        }
        else if (GUI == guiStats){
            if (ALLOWSTATSRESET){
                if (game.askToResetStats == false){
                    game.askToResetStats = true;
                }
                else{
                    game.stats = {0};
                    saveStatsEEPROM();
                    loadStatsEEPROM();
                    game.askToResetStats = false;
                }
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
                timeUPstart = millis();
                GUI = guiTimeUP;
                break;
            }
            game.update();
            game.draw();
            game.drawUI();

            display.display();
        } break;

        case (guiTimeUP):{
            drawTimeUP();
            display.display();

            if (millis() - timeUPstart >= 3000) {
                GUI = guiMenu;
            }
        } break;

        case (guiSettings):{
            drawSettings();
            display.display();
        } break;

        case (guiStats):{
            drawStats();
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

        game.stats.distanceReadTimes++;
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
    static Vector2 vel = {1.2, 1.5};
    const int size = 2;
    if (pos.x + vel.x > SCREEN_WIDTH || pos.x + vel.x < 0) vel.x *= -1;
    if (pos.y + vel.y > SCREEN_HEIGHT || pos.y + vel.y < 0) vel.y *= -1;
    pos.x += vel.x;
    pos.y += vel.y;
    display.drawRect(pos.x, pos.y, size, size, WHITE);
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
    display.drawLine(0, 14, SCREEN_WIDTH, 14, WHITE);

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

void drawStats(){
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Stats");
    display.drawLine(0, 14, SCREEN_WIDTH, 14, WHITE);                                                                   

    int padding = 4;
    int y_pos = 16;
    int x_pos = 0;
    display.setTextSize(1);

    // games played
    display.setCursor(x_pos, y_pos);
    display.print("Games played: ");
    display.print(game.stats.gamesPlayed);
    
    // totalGoals
    y_pos += padding + charSize_1.y;
    display.setCursor(x_pos, y_pos);
    display.print("Total goals: ");
    display.print(game.stats.totalGoals);

    // playtime
    y_pos += padding + charSize_1.y;
    display.setCursor(x_pos, y_pos);
    // display.print("Playtime: ");
    // display.print(game.stats.playtime);
    display.print("Dist read: ");
    display.print(game.stats.distanceReadTimes);
    
    if (game.askToResetStats){
        display.clearDisplay();
        display.setCursor(SCREEN_WIDTH / 2 - ((20 * charSize_1.x) / 2), SCREEN_HEIGHT / 2); // center text
        display.println("Press button 2 again");
        // centerTextAndPrint(display, "Press button 2 again", 20, charSize_1);
        display.setCursor(SCREEN_WIDTH / 2 - ((14 * charSize_1.x) / 2), SCREEN_HEIGHT / 2 + charSize_1.y + padding);  // center text
        display.print("to reset stats");
    }
}




void drawTimeUP(){
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor((SCREEN_WIDTH / 2 - ((8 * charSize_2.x) / 2)), (SCREEN_HEIGHT / 2 - ((1 * charSize_2.y) / 2)));   // center text
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

// void centerTextAndPrint(Adafruit_SSD1306 display, const Printable& text, int textWidth, Vector2 charsize) {
//     display.setCursor((SCREEN_WIDTH / 2 - ((textWidth * charsize.x) / 2)), (SCREEN_HEIGHT / 2 - ((1 * charsize.y) / 2)));
//     display.print(text);
// }


//TODO:
/*
-   BUGFIX: eventually paddles will end up at the top. Reason: "newPosCounter" combined with bad sensor readings when paddle not above sensor 
-   investigate: display's rows have small black padding with mega?
-   watchdog? <- is not strictly needed
-   score uart
-   ball friction
-   bugfix: ball should bounce back from the closest edge of the paddle (currently (atleast for p1) ball bounces back from the back edge when colliding)
-   button 1 and 2 input switches are a mess
-   move stuff to headers from main
-   cleanup
-   comments
*/