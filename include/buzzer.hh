#pragma once

namespace Buzzer {
    enum Sound {
        PADDLE,
        SCORE,
        SETTING
    };
    
    void initialization();
    void play(Sound sound);
}