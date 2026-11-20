#pragma once

namespace Buzzer {
    enum Sound {
        PADDLE,
        SCORE
    };
    
    void initialization();
    void play(Sound sound);
};