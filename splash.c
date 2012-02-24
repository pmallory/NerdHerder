/*
 *  Philip Mallory
 */
#include <stdlib.h>
#include "myLib.h"
#include "nerdLib.h"
#include "nerd_splash.h"

void splash(void);

extern int state;

void splash() {
    REG_DISPCTL = MODE3 | BG2_ENABLE;
    drawImage3(nerdbgBitmap, 0, 0, 160, 240);

    int rand_seed = 42;

    while (state == SPLASH) {
        rand_seed++;

        key_poll();
        if (key_hit(BUTTON_A))
            state = INSTRUCTIONS;
        else if (key_hit(BUTTON_B))
            state = GAME;
    }

    srand(rand_seed);
}

