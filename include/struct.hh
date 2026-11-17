#pragma once

#define DEBUG true
#define WHITE SSD1306_WHITE

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

// drawrect macro
#define drawRectHB(display, hitbox, c) display.drawRect((hitbox).x, (hitbox).y, (hitbox).width, (hitbox).height, (c))