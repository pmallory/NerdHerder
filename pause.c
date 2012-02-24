#include "myLib.h"
#include "nerdLib.h"
#include "pause_img.h"

extern int state;

void pause() {
    // draw background
    REG_DISPCTL = MODE3 | BG2_ENABLE;
    drawImage3(pause_imgBitmap, 0, 0, 160, 240);

    //wait for key press
    while (state == PAUSE) {
        key_poll();
        if (key_hit(BUTTON_SELECT))
            state = GAME;
    }

    // return to mode0
    REG_DISPCTL = MODE0 | BG3_ENABLE | SPRITE_ENABLE;
}

