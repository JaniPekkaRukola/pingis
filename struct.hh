#pragma once

#define DEBUG false

typedef struct Dir {
    bool right = false;
    bool up = false;
    bool stationary = true;
};

typedef struct Vector2 {
    float x;
    float y;
};

typedef struct Hitbox {
    float x;
    float y;
    float width;
    float height;
};